# GameServer

![GameServer](https://img.shields.io/badge/C%2B%2B-IOCP-blue.svg) ![GameServer](https://img.shields.io/badge/Godot-Network-green.svg)

게임 아이디어가 생각날 때마다 새로운 저장소를 만드는 대신, 하나로 몰아넣기위해 만든 레포지토리입니다.

1. StockGame : **Godot과 연동되는 IOCP 서버**를 개발하고 유지하기 위해 만든 프로젝트입니다.
처음엔 주식게임을 만들려고 생각했으나, Godot 이 매력적으로 보여 Godot 연결로 개발 중... 
시간 될 때, 주식 기능을 넣으려고 합니다.

## 🏆 프로젝트 개요
GameServer는 **Godot 게임 엔진과 통신할 수 있는 C++ 기반의 IOCP 서버**입니다. 높은 성능과 안정성을 목표로 하며, 다음과 같은 기능을 포함하고 있습니다.

- **IOCP 기반 비동기 네트워크 처리**
- **메모리 풀(Memory Pool) 사용**을 통한 효율적인 리소스 관리
- **std::weak_ptr 활용**하여 스마트 포인터 기반 메모리 관리
- **Godot과의 패킷 통신** 지원

이 프로젝트는 **MMORPG 및 실시간 멀티플레이어 게임 개발을 위한 서버 아키텍처를 탐색하는 실험적인 목적**을 갖고 있습니다.

## 🚀 기술 스택
- **언어**: C++20
- **네트워크**: Windows IOCP (I/O Completion Port)
- **메모리 관리**: std::shared_ptr 을 이용한 Memory Pool
- **연동 엔진**: Godot (C++ GDExtension 활용 가능성 고려)
- **데이터 직렬화**: Protobuf (예정)

## 📂 프로젝트 구조
```
GameServer/
├── Core/
│   ├── Network/	# 네트워크 관련 코드 (IOCP, 패킷 처리)
│   ├── Memory/	# 메모리 풀 및 스마트 포인터 관리
│   ├── DB/ 		# 게임 로직 처리
│   ├── Thread/	# 스레드 관리
├── Game/ 		# 문서 및 API 설명
└── README.md	# 프로젝트 개요 및 가이드
```

## 📡 Godot과의 연결 (예정)
Godot과의 네트워크 연동을 위해 다음과 같은 방식이 고려되고 있습니다.
1. **WebSocket 또는 TCP/IP 직접 연결**을 통해 IOCP 서버와 통신
2. **Godot의 GDExtension을 활용하여 C++ 네이티브 바인딩 구현**
3. **Protobuf을 활용한 데이터 직렬화 및 역직렬화 지원** (예정)

## 📌 향후 개발 계획
- ✅ 기본적인 **IOCP 서버 구조** 구현
- ✅ **메모리 풀 및 스마트 포인터 최적화**
- 🔄 **Godot과의 직접적인 TCP 연결 테스트**
- 🔄 **게임 로직 구현** (간단한 채팅 서버 or 멀티플레이 기능)
- 🔄 **데이터 직렬화 포맷 적용 (Protobuf 예정)**

## 📬 기여 및 문의
이 프로젝트는 개인 연구 및 실험용으로 개발되고 있으며, 피드백과 기여를 환영합니다. 궁금한 점이 있다면 **Issues** 또는 **Pull Requests**를 활용해주세요!

---

© 2025 CodingPythonMan. All Rights Reserved.