# GameServer

![GameServer](https://img.shields.io/badge/C%2B%2B-IOCP-blue.svg) ![GameServer](https://img.shields.io/badge/Godot-Network-green.svg)

게임 아이디어가 생각날 때마다 새로운 저장소를 만드는 대신, 하나로 몰아넣기위해 만든 레포지토리입니다.

## 1. C++ / StockGame : IOCP 서버
### 🏆 프로젝트 개요
**[Godot 게임 엔진](https://github.com/CodingPythonMan/GodotResearch)과 통신할 수 있는 C++ 기반의 IOCP 서버**입니다. 높은 성능과 안정성을 목표로 하며, 다음과 같은 기능을 포함하고 있습니다.
- **IOCP 기반 비동기 네트워크 처리**
- **메모리 풀(Memory Pool) 사용**을 통한 효율적인 리소스 관리
- **std::weak_ptr 활용**하여 스마트 포인터 기반 메모리 관리
- **Godot과의 패킷 통신** 지원

이 프로젝트는 **MMORPG 및 실시간 멀티플레이어 게임 개발을 위한 서버 아키텍처를 탐색하는 실험적인 목적**을 갖고 있습니다.

### 🚀 기술 스택
- **언어**: C++20
- **네트워크**: Windows IOCP (I/O Completion Port)
- **메모리 관리**: std::shared_ptr 을 이용한 Memory Pool
- **연동 엔진**: Godot (C++ GDExtension 활용 가능성 고려)
- **데이터 직렬화**: Protobuf (예정)

### 📂 프로젝트 구조
```
StockGame/
├── Core/
│   ├── Network/	# 네트워크 관련 코드 (IOCP, 패킷 처리)
│   ├── Memory/	# 메모리 풀 및 스마트 포인터 관리
│   ├── DB/ 		# 게임 로직 처리
│   ├── Thread/	# 스레드 관리
├── Game/ 		# 문서 및 API 설명
└── README.md	# 프로젝트 개요 및 가이드
```

## 2. C# / LogServer : Akka.net 서버
### 🏆 프로젝트 개요
원격 로그 서버입니다. 멀티스레드, 비동기로 대규모 처리하기 위해 만들었습니다. Akka 인만큼 액터 기반입니다.
액터 많은 수는 DB 저장에 쓰입니다.

- **WithRouter(new SmallestMailboxPool(30)** 로 일감 액터 별로 분배

### 📬 기여 및 문의
이 프로젝트는 개인 연구 및 실험용으로 개발되고 있으며, 피드백과 기여를 환영합니다. 궁금한 점이 있다면 **Issues** 또는 **Pull Requests**를 활용해주세요!

---

© 2025 CodingPythonMan. All Rights Reserved.