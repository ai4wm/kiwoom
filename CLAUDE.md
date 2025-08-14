# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## 프로젝트 개요

키움증권 OpenAPI를 활용한 C++ MFC 테스트 애플리케이션입니다. 키움 ActiveX 컨트롤을 래핑하여 주식 시세 조회, 주문 관리, 실시간 데이터 수신 기능을 제공합니다.

## 빌드 및 개발 명령어

### 프로젝트 빌드
```bash
# Visual Studio에서 빌드
devenv KhOpenApiTest.sln /build Debug
# 또는 MSBuild 사용
MSBuild KhOpenApiTest.sln /p:Configuration=Debug /p:Platform=Win32
```

### 빌드 구성
- Debug/Release 구성, Win32 플랫폼
- 실행파일: `Debug/KhOpenApiTest.exe` 또는 `Release/KhOpenApiTest.exe`

### 개발 환경 요구사항
- Visual Studio 2022 (v143 toolset)
- MFC 라이브러리 (Debug: 동적 연결, Release: 정적 연결)
- Windows 10 SDK
- 키움 OpenAPI 설치 필수

## 핵심 아키텍처

### 메인 컴포넌트
- **메인 다이얼로그**: `CKhOpenApiTestDlg` - 메인 창 및 API 이벤트 처리
- **ActiveX 래퍼**: `CKHOpenAPICtrl` - 키움 COM 인터페이스 래핑
- **서브 다이얼로그**: 각 기능별 전용 다이얼로그
- **그리드 컨트롤**: `CGridCtrl` - 시세 데이터 표시용 커스텀 그리드

### 주요 파일 구조

#### ActiveX 통합 레이어
- `KHOpenAPI.h/cpp` - 키움 API 메소드 래퍼
- `KHOpenAPICtrl.h/cpp` - ActiveX 컨트롤 관리 클래스
- 주요 메소드: `CommConnect()`, `CommRqData()`, `SendOrder()`, `GetLoginInfo()`

#### 다이얼로그 클래스
- `KhOpenApiTestDlg.h/cpp` - 메인 애플리케이션, API 이벤트 핸들러
- `CurrentPriceDlg.h/cpp` - 현재가 조회 다이얼로그
- `OrderDlg.h/cpp` - 주문 관리 다이얼로그  
- `KwanSimDlg.h/cpp` - 관심종목 관리
- `RealAddDlg.h/cpp` - 실시간 데이터 등록 다이얼로그
- `RateDlg.h/cpp` - 비율/수익률 계산

#### 이벤트 처리 시스템
메인 다이얼로그의 핵심 이벤트 핸들러:
- `OnReceiveTrDataKhopenapictrl()` - TR 응답 처리
- `OnReceiveRealDataKhopenapictrl()` - 실시간 데이터 수신
- `OnReceiveMsgKhopenapictrl()` - 메시지 처리
- `OnReceiveChejanData()` - 체결 데이터 수신
- `OnEventConnect()` - 연결 상태 처리

#### 화면번호 관리 시스템
- `m_mapScreen` - 다중 화면번호 관리
- `m_mapScreenNum` - 화면번호 할당 및 추적
- `GetNextScreenNum()` - 동적 화면번호 생성

### 그리드 컨트롤 시스템
- `GridCtrl/` 폴더: 포괄적인 그리드 컨트롤 구현
- `CGridCtrl` - 시세 데이터 표시용 메인 그리드 클래스
- 기능: 인플레이스 편집, 드래그앤드롭, 커스텀 렌더링

### 데이터 플로우
1. **연결**: `CommConnect()`로 키움 서버 접속
2. **인증**: 키움 로그인 다이얼로그를 통한 사용자 인증
3. **데이터 요청**: `CommRqData()`로 화면번호와 함께 시세 요청
4. **응답 처리**: 이벤트 핸들러에서 수신 데이터 파싱 및 표시
5. **실시간 업데이트**: `OnReceiveRealDataKhopenapictrl()`에서 실시간 데이터 스트림 처리

## 개발 참고사항

### API 통합 패턴
- 모든 API 호출은 비동기 콜백 기반
- 동시 요청시 화면번호 중복 방지 필수
- 콜백의 `sErrorCode` 파라미터로 에러 처리
- 실시간 데이터는 등록/해제 관리 필요

### MFC 다이얼로그 관리
- 메인 다이얼로그에서 자식 다이얼로그 생성
- `m_pRealAddDlg` 등의 포인터로 자식 다이얼로그 생명주기 관리
- 메시지 맵으로 MFC 표준 및 ActiveX 커스텀 이벤트 처리

### 문자 인코딩
- MultiByte 문자셋 사용 (유니코드 아님)
- 한국어 주석 및 문자열 다수 포함
- 리소스 컬쳐: 한국어(0x0412)

### 관리자 권한
- 애플리케이션 실행시 관리자 권한 필수
- 키움 API 통합 및 시스템 레벨 작업을 위해 필요