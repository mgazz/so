// Package main provides ...
package main

import (
	"log"
	"time"
)

func son(ch chan string) {
	log.Println("son started")
	for {
		m,ok:= <-ch
		if ok {
			log.Println("message m:",m," ok: ",ok)
		}
		if m=="stop" && ok {
			log.Println("closing the channel")
			ch <- "stopped"
			//chiusura del canale
			close (ch)
			break
		}

	}
}
func main() {
	ch:= make( chan string)
	log.Println("start son")
	go son(ch)
	log.Println("start sleep")
	time.Sleep(5*time.Second)
	ch <- "stop"
	for v := range ch { 
		log.Println("closed channel with value: ",v)
	}
	//for  {
		//v,ok:= <-ch
		//if ok {
			//log.Println("message m:",v," ok: ",ok)
		//}
		//if v=="stopped" && ok {
			//log.Println("channel closed")
		//}else {
			//break
		//}
	//}
	return
}
