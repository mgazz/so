package main

import (
	"log"
	"time"
)

//la clausola range nel for ripete la receive dal canale specificato 
//fino a che il canale non viene chiuso

func son(ch chan string) {
	log.Println("son started")
	for {
		m,ok:= <-ch
		log.Println("message m:",m," ok: ",ok)
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
	for  {
		v,ok:= <-ch
		if ok {
			log.Println("message m:",v," ok: ",ok)
		}
		if v=="stopped" && ok {
			log.Println("channel closed")
			break
		}
	}
	return
	
}
