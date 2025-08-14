# 키움증권 OpenAPI 테스트 애플리케이션

키움증권 OpenAPI를 활용한 C++ MFC 기반 주식 거래 시스템입니다.

## 프로젝트 개요

이 프로젝트는 키움증권의 OpenAPI를 사용하여 주식 시세 조회, 주문 관리, 실시간 데이터 수신 기능을 제공하는 데스크톱 애플리케이션입니다.

## 주요 기능

### 📈 시세 조회
- 실시간 주식 시세 조회
- 호가 정보 표시
- 체결 정보 확인
- 차트 데이터 조회

### 💼 주문 관리
- 일반 주식 주문 (매수/매도)
- 시간외 주문 처리
- 신용거래 주문
- 주문 내역 조회 및 관리

### 📊 관심종목 관리
- 관심종목 등록/삭제
- 실시간 시세 모니터링
- 수익률 계산

### 🔄 실시간 데이터
- 실시간 시세 수신
- 체결 데이터 알림
- 계좌 잔고 변동 알림

## 개발 환경

- **개발 언어**: C++ (MFC)
- **IDE**: Visual Studio 2022
- **플랫폼**: Windows 10/11
- **아키텍처**: Win32
- **필수 요구사항**: 키움 OpenAPI 설치

## 빌드 방법

### 사전 준비
1. Visual Studio 2022 설치 (MFC 라이브러리 포함)
2. Windows 10 SDK 설치
3. 키움증권 OpenAPI 설치

### 빌드 명령
```bash
# Visual Studio에서 빌드
devenv KhOpenApiTest.sln /build Debug

# 또는 MSBuild 사용
MSBuild KhOpenApiTest.sln /p:Configuration=Debug /p:Platform=Win32
```

### 빌드 구성
- **Debug/Release**: 디버그/릴리스 구성
- **플랫폼**: Win32
- **출력**: `Debug/KhOpenApiTest.exe` 또는 `Release/KhOpenApiTest.exe`

## 프로젝트 구조

```
KhOpenApiTest/
├── src/
│   ├── KhOpenApiTestDlg.h/cpp      # 메인 다이얼로그
│   ├── CurrentPriceDlg.h/cpp       # 현재가 조회
│   ├── OrderDlg.h/cpp              # 주문 관리
│   ├── KwanSimDlg.h/cpp           # 관심종목 관리
│   ├── RealAddDlg.h/cpp           # 실시간 데이터 등록
│   └── RateDlg.h/cpp              # 수익률 계산
├── ActiveX/
│   ├── KHOpenAPI.h/cpp            # API 래퍼
│   └── KHOpenAPICtrl.h/cpp        # ActiveX 컨트롤
├── GridCtrl/                      # 그리드 컨트롤
└── res/                          # 리소스 파일
```

## 핵심 아키텍처

### ActiveX 통합
- **KHOpenAPICtrl**: 키움 ActiveX 컨트롤 래퍼
- **비동기 콜백**: 모든 API 호출은 비동기 이벤트 기반
- **화면번호 관리**: 중복 방지를 위한 화면번호 시스템

### 이벤트 처리
- `OnReceiveTrDataKhopenapictrl()`: TR 응답 처리
- `OnReceiveRealDataKhopenapictrl()`: 실시간 데이터 수신
- `OnReceiveMsgKhopenapictrl()`: 메시지 처리
- `OnReceiveChejanData()`: 체결 데이터 수신

### 데이터 플로우
1. **연결**: `CommConnect()`로 키움 서버 접속
2. **인증**: 키움 로그인 시스템
3. **데이터 요청**: `CommRqData()`로 시세 요청
4. **실시간 등록**: 실시간 데이터 스트림 수신

## 사용 방법

### 1. 애플리케이션 실행
- 관리자 권한으로 실행 (키움 API 요구사항)
- 키움 로그인 후 사용 가능

### 2. 주요 기능 사용
- **현재가 조회**: 종목코드 입력 후 조회
- **주문**: 계좌번호, 종목코드, 수량, 가격 입력 후 주문
- **시간외주문**: 시간외 거래 시간에 주문 처리
- **관심종목**: 종목 등록 후 실시간 모니터링

### 3. 실시간 데이터
- 원하는 종목의 실시간 데이터 등록
- 체결, 호가 정보 실시간 업데이트

## 주의사항

⚠️ **중요**: 이 애플리케이션은 실제 주식 거래를 수행합니다. 테스트 시에는 모의투자 계좌를 사용하시기 바랍니다.

- 키움증권 계좌 및 OpenAPI 사용 신청 필요
- 관리자 권한으로 실행 필수
- 시장 운영 시간에만 실시간 데이터 수신 가능
- 일일 API 호출 제한 주의

## 라이선스

이 프로젝트는 키움증권 OpenAPI를 사용하므로 키움증권의 이용약관을 준수해야 합니다.

## 기여하기

1. Fork the Project
2. Create your Feature Branch (`git checkout -b feature/AmazingFeature`)
3. Commit your Changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to the Branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request

## 문의사항

프로젝트 관련 문의사항이 있으시면 Issues 탭에 등록해 주세요.

---

**개발자**: AI4WM Team  
**최종 업데이트**: 2025년 1월