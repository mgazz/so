package main

import (
	"log"
	"time"
)

func figlioint(ch chan int) {
	time.Sleep(4*time.Second)
	ch<-10
	close(ch)
}

func figliostr(ch chan string) {
	time.Sleep(3*time.Second)
	ch<-"ciao"
	close(ch)
}

func main() {
	ci,cs := make ( chan int), make (chan string)
	go figlioint(ci)
	go figliostr(cs)
	for  {
		select {
		case v:=<-ci:
			if v==0 {
				return
			}
			log.Printf("ricevuto %d da ci\n",v)
		case v:=<-cs:
			log.Printf("ricevuto %s da cs\n", v)
			if len(v)==0{
				return
			}
		}
	}
}
