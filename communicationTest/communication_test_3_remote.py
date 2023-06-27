import requests
while True: 
    x=input("MASUKKAN KODE : ")
    r = requests.post('http://192.168.55.241/arah', json={"arah" : x}) 
    requests.post('http://192.168.55.241/arah', json={"arah" : 0}) 
    print(r.text)