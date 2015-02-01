// Package main provides ...
package main

import (
	"fmt"
	"time"
)

//NB: default i canali non sono bufferizzati e comunicazione sincrona bloccante
//NB: la receive da un canle non inizializzato (nil) is bloccante

//mod inserimento nel canale
func partenza(ch chan<-int, sync1 <-chan bool, sync2 chan <- bool) {

	//posso usare le variabili che sono visibili nella funzione

	go func() {
		for i := 0; ; i++ {
			ch <-i //invia stampa
			time.Sleep(1e9)
		}
	}()
	//aspetto stop
	<-sync1
	close(ch)
	//comunico ricevuto stop
	sync2<-true
}

// mod estraz dal canale
func arrivo(ch <-chan int) {

	for i := 0; ; i++ {
		v,ok := <-ch
		if ok {
			fmt.Println("ricevuto: ", v) //ricevi stampa
		} else {
			fmt.Println("canale chiuso do partenza")
			break;
		}
	}

}

func createChan() chan int{
	var channel chan int  
	channel = make(chan int)
	go func() {
			for i := 0; ; i++ {
				channel <- i
			}
	}()
	return channel
}

func main() {

	var ch chan int
	ch = make(chan int )

	var sync1 chan bool
	sync1 = make(chan bool)

	var sync2 chan bool
	sync2 = make(chan bool)

	//mychannel := createChan()
	//fmt.Println(<-mychannel)
		
	go partenza(ch,sync1,sync2)
	go arrivo(ch)

	time.Sleep(3*1e9)

	fmt.Println("send stop");
	sync1<-true
	<-sync2
	fmt.Println("recived response done!");


}

