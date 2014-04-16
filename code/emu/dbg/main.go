package main

import (
	"fmt"
	"net/http"
	"sync"
	"io"
	"log"
	"time"
)


func main() {
	go scrapeData()
	fmt.Println("Inited!")
	http.HandleFunc("/hello", HelloServer)
	err := http.ListenAndServe(":1123", nil)
	if err != nil {
		log.Fatal("ListenAndServe: ", err)
	}
}

func scrapeData() {
	for {
		time.Sleep(time.Second/4)
		
		v, err := http.Get("http://192.168.3.20/dbg.bin")
		if err != nil {
			fmt.Println("Error in GET: ", err)
			time.Sleep(time.Second*3)
			continue
		}
		if v.Status != "200 OK" {
			fmt.Println("GET: ", v.Status)
			time.Sleep(time.Second*3)
			continue
		}
		
		var buf [3]byte
		if _,err := io.ReadFull(v.Body, buf[:]); err != nil {
			fmt.Println("Error reading data from dbg.bin:", err)
			continue
		}
		fmt.Println(buf)
		data.Lock()
		data.Front.Append(int(buf[0]))
		data.Right1.Append(int(buf[1]))
		data.Right2.Append(int(buf[2]))
		data.Unlock()
		v.Body.Close()
	}
}

type Data struct {
	sync.Mutex
	Front, Right1, Right2 LineGraph
}
var data Data

func HelloServer(w http.ResponseWriter, req *http.Request) {
	w.Header().Add("Content-Type", "text/html")
	
	/*req.ParseForm()
	se := make(map[string]string, len(req.Form))
	for k,v := range req.Form {
		se[k] = v[0]
	}*/

	data.Lock()
	defer data.Unlock()
	
	fmt.Fprint(w, "<h1>Front sensor</h1>")
	data.Front.Html(w)
	
	fmt.Fprint(w, "<h1>Right sensor 1</h1>")
	data.Right1.Html(w)
	
	fmt.Fprint(w, "<h1>Right sensor 2</h1>")
	data.Right2.Html(w)
}






