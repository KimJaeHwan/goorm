*** 해결 완료 ^오^ ***
recv 로 수신시 send한 데이터 만큼 받아야하며 
보낸만큼 전부 받지않으면 다음 recv때 나머지를 수신하게 된다....
---



일단 파일 다운로드 하는 부분에서 정상적으로 다운로드가 되나 서버측의 수신 버퍼측의 오류로 예상되는 의도하지 않는 recv가 발생한다.

해당 되는 에러는 client에서 connect하자마자 2. Download File > list 으로 파일을 다운 받는경우 의도치 않는 recv를 하여 null을 수신하는것을 확인했다.

이는 recv의 수신 버퍼에 null이 남아 있어 발생하는 것으로 추측된다.

따라서 위의 에러는 1. Get Current Time을 이용하거나 3.Echo Server을 이용한후 2. Download File을 하면 1회는 정상적으로 된다.

다음 1회 다운로드 이후 2번째 파일을 연속으로 다운로드 시도하는 경우 또한 의도치않게 recv를 하여 null을 수신한다.

따라서 이는 3. Go back 을 이용하여 밖에 나갔다가 다시 2.Download File을 했을때 정상적으로 다운로드를 실행한다.

------

![image](https://user-images.githubusercontent.com/45413267/116774422-72ef4880-aa97-11eb-8d31-657414ec5adf.png)

위의 이미지는 첫번째 에러를 나타낸 server측의 출력문이다...

1번을 정상적으로 수신하고  Book.txt를 정상적으로 연 모습이다 파일 또한 정상적으로 전송하여 file send complete!! 와같이 나타내어주고 있다. 

이후 비정상적인 NULL을 recv하여 정상적이지 못한 부분을 참조해버리는 모습이다.

------

![image](https://user-images.githubusercontent.com/45413267/116774522-0cb6f580-aa98-11eb-869c-3441a1277fc4.png)

두번째 오류이다.

1번을 이용하여 Current Time을 조회한후 2. Download File 을 이용하여 1번 Book.txt를 다운로드 받는다, 이후 연속으로 2번 HallymUniv.jpg 파일을 다운로드를 시도하는경우 정상적으로 파일을 다운로드하나 Server측에서 NULL을 recv하여 프로세스가 진행 되지 않는 모습이다.
