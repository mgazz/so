package main

import (
	"log"
	"time"
	"sync"
)
//NB: essendo c<-10 nel main non sospensivo viene prima stanpato inviato e poi ricevuto
//NB: ho utilizzato sync per aspettare la routine
func main() {
	var wg sync.WaitGroup
	c := make ( chan int, 50)
	wg.Add(1)
	go func() {
		defer wg.Done()
		//time.Sleep(60*1e9)
		time.Sleep(3*time.Second)
		x := <-c
		log.Println("ricevuto",x)
	}()
	log.Println("sending",10)
	c<-10 //asincrona viene subito eseguita la prossima istruzione
	log.Println("inviato",10)
	wg.Wait()
}
