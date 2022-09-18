# SmartValve
<div>
<img src="https://img.shields.io/badge/Arduino-00979D?style=flat-square&logo=Arduino&logoColor=white"/>
<img src="https://img.shields.io/badge/HTML-E34F26?style=flat-square&logo=HTML5&logoColor=white"/>
<img src="https://img.shields.io/badge/Javascript-F7DF1E?style=flat-square&logo=javascript&logoColor=black"/>
<img src="https://img.shields.io/badge/PHP-777BB4?style=flat-square&logo=PHP&logoColor=white"/>
<img src="https://img.shields.io/badge/MySQL-4479A1?style=square-square&logo=MySQL&logoColor=white"/>
</div>

## LoRa 통신기반 자가발전형 원격 수로 모니터링 시스템

<div align="center">
<img src="https://github.com/cjk09083/SmartValve/blob/main/%EC%82%AC%EC%A7%84%26%EC%98%81%EC%83%81/1.%20%ED%99%88%ED%8E%98%EC%9D%B4%EC%A7%80%20%EB%A9%94%EC%9D%B8.png" width="45%"/>&nbsp;  
<img src="https://github.com/cjk09083/SmartValve/blob/main/%EC%82%AC%EC%A7%84%26%EC%98%81%EC%83%81/2.%20홈페이지%20상세.png" width="45%"/>
</div></br>


## 목적
- 수로에 흐르는 물을 이용해 자가발전 
- 수로와 사무실간의 LoRa를 이용한 무선통신을 연결하고 데이터 송수신 하는 시스템 
- 수로의 상태를 홈페이지에서 모니터링하고 홈페이지에서 수로를 원격 제어

## 담당 
- 모니터링 홈페이지 (HTML, PHP, JavaScript)  
- 아두이노의 수로 모터 제어와 센서링 정보 무선 통신 (LoRa, WiFi)
- KC 인증 (전자파적합성인증 - 한국전자통신연구원)
- 소수력 발전을 위한 충방전 회로 설계 

## 기능

### 1. 수로에서 흐르는 물로 발전기를 돌려 아두이노와 센서들을 동작시킬 전력 충전
 - 수로 내에 수차와 발전기를 연결, 발생하는 전력을 변환하여 배터리에 충전

### 2. 수로에서 측정한 정보를 서버와 연동
 - 수로에 설치된 아두이노와 사무실에 설치된 아두이노간의 LoRa 통신 연결 (무자격으로 사용 가능한 923.1MHz 대역 사용)
 - 사무실에 설치된 아두이노에서 서버로 정보 업로드 (Wifi HTTP)
 - 홈페이지에서 밸브 OPEN량을 0~100%로 조절하면 통신이 역순으로 수로에 도달해 수로의 수문에 달린 모터 제어

<div align="center">
<img src="https://github.com/cjk09083/SmartFarm/blob/main/%EC%82%AC%EC%A7%84%20%EB%B0%8F%20%EC%98%81%EC%83%81/%EB%86%8D%EC%9E%A5%20%EB%82%B4%EB%B6%80.jpg" width="60%"/>
</div></br>

