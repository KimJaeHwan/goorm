일단 파일 다운로드 하는 부분에서 정상적으로 다운로드가 되나 서버측의 수신 버퍼측의 오류로 예상되는 의도하지 않는 recv가 발생한다.

해당 되는 에러는 client에서 connect하자마자 2. Download File > list 으로 파일을 다운 받는경우 의도치 않는 recv를 하여 null을 수신하는것을 확인했다.

이는 recv의 수신 버퍼에 null이 남아 있어 발생하는 것으로 추측된다.

따라서 위의 에러는 1. Get Current Time을 이용하거나 3.Echo Server을 이용한후 2. Download File을 하면 1회는 정상적으로 된다.

다음 1회 다운로드 이후 2번째 파일을 연속으로 다운로드 시도하는 경우 또한 의도치않게 recv를 하여 null을 수신한다.

따라서 이는 3. Go back 을 이용하여 밖에 나갔다가 다시 2.Download File을 했을때 정상적으로 다운로드를 실행한다.
