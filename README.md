# docker_project
prometheus 를 활용한 cpu 자원 관리, ping 을 활용한 health check

### health check
- 설정
1. json library 설치 : setting 폴더의 json_build.sh 파일 실행
2. /usr/local/lib : 라이브러리 경로 추가
3. json/health_container.json 컨테이너 정보 수정
4. health 실행 시 슈퍼 유저 권한 필요

- 동작
1. health_container.json 파일에 저장된 컨테이너 정보(name, bridge)를 추출한다.
2. 컨테이너의 IP 주소를 docker inspect [option]을 사용하여 컨테이너의 private IP address를 조회한다.
3. 컨테이너에 PING ICMP 패킷을 전송하여 컨테이너의 상태를 확인한다. (health check)
4. 타임 아웃 되거나 연결에 문제가 발생하였다면 문제가 발생한 컨테이너를 docker restart 를 통해서 재가동한다.
5. 2의 과정으로 돌아가 지속적으로 health check를 수행한다. 


### resource manager
- 설정
1. json library 설치 : setting 폴더의 json_build.sh 파일 실행
2. /usr/local/lib : 라이브러리 경로 추가
3. json/health_container.json 컨테이너 정보 수정
4. cadvisor 실행
5. prometheus.yml 파일 수정
6. prometheus 실행

- 동작
1. cadvisor를 통해 수집된 컨테이너 메트릭을 prometheus로 전송한다.
2. prometheus에 수집된 모니터링 데이터를 curl [url/query]를 통해서 json 파일의 데이터를 조회한다.
3. 모니터링으로 통해 수집된 정보를 통해서 프로그램 된 정책에 의해 컨데이너에게 자원을 할당한다.
4. 1의 과정으로 돌아가 지속적으로 컨테이너 자원 사용율을 관리한다.

### setting
cadvisor 컨테이너 실행
```groovy
docker run \
  --volume=/:/rootfs:ro \
  --volume=/var/run:/var/run:rw \
  --volume=/sys:/sys:ro \
  --volume=/var/lib/docker/:/var/lib/docker:ro \
  --volume=/dev/disk/:/dev/disk:ro \
  --publish=8080:8080 \
  --detach=true \
  --name=cadvisor \
  google/cadvisor:latest
```

prometheus 컨테이너 실행 예시
```groovy
docker run -d --name prometheus -p 9090:9090 -v /etc/prometheus/prometheus.yml:/etc/prometheus/prometheus.yml prom/prometheus
```

Error while loading shared libraries: libxxxx.so 발생 시
```groovy
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:~/lib
```
만약 위의 명령으로 해결이 되지 않는다면
/etc/ld.so.conf.d/ 디렉토리에 libxxxx.conf 파일을 만들어서 이 파일에 /usr/local/lib 를 지정해둔다.
그리고 난 뒤에 sudo /sbin/ldconfig 를 실행하여 갱신한다.
