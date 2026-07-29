# WS2812B RGB LED 드라이버 트러블슈팅 노트

STM32F411 + WS2812B 4구 보드(RGB) 드라이버 개발 중 겪은 문제와 원인, 해결 기록.

## 환경

- MCU: STM32F411 (SYSCLK 96MHz, TIM2CLK 96MHz)
- WS2812B 4구 보드, DIN -> PA1 (AF01, TIM2_CH2)
- 프로토콜: 800KHz(1.25us) PWM, 0-code/1-code Duty 변조, GRB 순서 MSB first

## 이슈 1: PA2 사용 시 신호가 아예 안 나감

- 처음엔 PA2(AF01, TIM2_CH3)를 사용.
- 원인: `Uart2_Init()`이 PA2/PA3를 USART2(AF07)로 설정한 뒤 `GPIOA->LCKR` 로 GPIO 설정을 리셋 전까지 Lock 해버림.
- `Sys_Init()`에서 `Uart2_Init()`이 먼저 실행되기 때문에, 이후 `RGB_LED_Init()`이 PA2를 AF01로 바꾸려 해도 무시되고 PA2는 계속 USART2_TX로 남아있었음.
- **해결**: PA1(TIM2_CH2)으로 핀 변경. UART2(PA2/PA3)와 물리적으로 겹치지 않음.

## 이슈 2: 전원/신호 품질 관련 삽질

- LED 보드 VDD를 3.3V로 구동(5V 라인과 쇼트, 강사님 지시).
- WS2812B 정격 최소 VDD는 3.5V라 스펙 아웃이었지만, 실측 결과 3.3V로도 정상 동작 확인됨.
- 마지막(4번째) LED에서만 색이 이상하게 나오는 현상이 있어 한동안 "특정 LED 하드웨어 결함"으로 의심하고 디버깅(전원 분리 테스트, 단독 점등 테스트, 시리즈 저항 검토 등)했으나, 최종적으로는 하드웨어 결함이 아니라 아래 이슈3(초기 안정화 딜레이 부족)이 원인이었음.

## 이슈 3: 전원 인가 직후 첫 프레임 색이 틀어짐

- `RGB_LED_Init()` 직후 딜레이 없이 바로 색 데이터를 전송하면, 첫 프레임만 색이 틀어지고 이후 프레임은 정상인 현상 발생.
- **해결**: Init 직후 Off(0,0,0) 프레임을 한 번 보내고 약 100ms 대기한 뒤 실제 색상 전송 시작.

## 이슈 4: DMA+PWM 전환 후 "아예 안 나옴"

CPU 폴링 방식(비트마다 CCR 값을 CPU가 직접 쓰고 대기)은 정상 동작을 확인한 뒤, 실시간성 확보를 위해 DMA1_Stream1(Channel 3, TIM2_UP)이 자동으로 CCR2에 값을 흘려주는 방식으로 전환. 전환 직후 LED가 전혀 켜지지 않는 문제 발생, 아래 순서로 원인 분석:

1. **DMA 채널/스트림 매핑 재검증**: RM0383 Table 27(DMA1 request mapping)로 Stream1/Channel3 = TIM2_UP 매핑 확인 → 정상.
2. **레지스터 값 직접 확인**: 디버그 프린트로 `DMA1_Stream1->CR`, `TIM2->DIER` 값을 찍어 CHSEL/MSIZE/PSIZE/MINC/DIR/TCIE/UDE 비트가 의도한 그대로 설정됨을 확인 → 정상.
3. **실제 전송 소요 시간 측정**: TIM4를 1us 자유 카운터로 사용해 DMA 시작~완료(Transfer Complete) 시간을 측정 → 177us (설계값 141×1.25us=176.25us와 정확히 일치) → 타이밍도 정상.
4. **GPIO Output↔AF 전환 로직 점검**: DMA 버전에서 "처음부터 AF 고정"으로 바꿨다가, 폴링 버전과 동일하게(Idle 시 GPIO Output Low, 전송 시에만 AF 전환) 되돌림 → 그래도 문제 지속.
5. **최종 원인 발견**: `TIM2->CCR2`는 32bit 레지스터인데, DMA를 16bit(half-word) 단위(MSIZE=PSIZE=01b)로 설정해서 쓰고 있었음. DMA 컨트롤러 자체는 141개 half-word 전송을 문제없이 완료(NDTR=0)했다고 보고했지만, 32bit 전용 레지스터에 half-word 단위 쓰기가 실제로는 제대로 반영되지 않았던 것으로 추정.
6. **해결**: DMA `MSIZE`/`PSIZE`를 32bit(word, 10b)로 변경하고 버퍼 타입도 `unsigned short` → `unsigned int` 로 변경 → 정상 동작 확인.

## 최종 구조

- `RGB_LED_Init()`: GPIO/TIM2 레지스터 1회 설정 (Idle: PA1 GPIO Output Low, TIM2 Clock Off)
- `RGB_LED_Enable()` / `RGB_LED_Disable()`: 전송 직전/직후에만 TIM2 Clock On + PA1을 AF로 전환 / 그 반대
- `RGB_LED_Send_All()` / `RGB_LED_Send_One()`: 버퍼(24bit×LED개수 + Reset용 0) 채우고 DMA1_Stream1로 전송 시작, **즉시 리턴(non-blocking)**
- 전송 완료 처리는 `DMA1_Stream1_IRQHandler`(exception.c)에서 `RGB_LED_Disable()` 호출로 마무리
- `RGB_LED_Is_Busy()`로 전송 중 여부 확인 가능, 전송 중 재호출 시 무시(non-blocking 유지)

## 교훈

- DMA가 "성공"을 보고해도(NDTR=0, Transfer Complete) 대상 레지스터의 접근 크기(MSIZE/PSIZE)가 레지스터 폭과 맞지 않으면 실제로는 값이 반영되지 않을 수 있다. 32bit 레지스터에는 32bit 단위로 접근하는 것이 안전하다.
- GPIO 모드 전환(Output↔AF) 자체는 문제의 본질이 아니었지만, 안전하게 Idle 상태를 명시적으로 보장하는 습관은 유지할 가치가 있다.
- 하드웨어 결함으로 보였던 증상(마지막 LED 색 이상)이 실제로는 타이밍/소프트웨어 문제였던 경우가 있었음 — 유사 증상이 재발하면 소프트웨어 쪽부터 재검증할 것.
