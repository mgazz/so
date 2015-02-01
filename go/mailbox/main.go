// Package main provides ...
package main

import (
	"fmt"
	//"time"
)
const (
	max_prod = 2
	max_cons = 2
	max_stage = 10
)

var channel[max_stage]chan int
var done_cons = make(chan int )
var done_prod= make(chan int )
var done_server= make(chan int )

var end = make(chan bool)

func produttore(id int,in_ch chan int) {
	fmt.Printf("produttore [%d] invia id su canale \n", id)
	in_ch <-id
	done_prod<-id
	fmt.Printf("produttore [%d] stoppa\n",id)
}

func consumatore(id int, out_ch chan int) {
	message := <-out_ch
	fmt.Printf("consumatore [%d] ricevuto messaggio: %d\n", id,message)
	done_cons<-id
	fmt.Printf("consumatore [%d] stoppa\n",id)
}

func server(n_ch int, in_ch chan int , out_ch chan int) {
	for  {
		select {
		case message := <-in_ch:
			fmt.Printf("stage [%d] - ricevuto messaggio [%d]\n", n_ch,message)
			out_ch<-message
		case <-end:
			fmt.Printf("server [%d] termina\n",n_ch )
			done_server <-n_ch
			return
		}
	}
}

func main() {
	var n_stage int
	fmt.Printf("+++++++		start main		+++++++\n")
	fmt.Printf("inserire il numero di stage\n")
	fmt.Scanf("%d/n",&n_stage)

	//n canali + 1 per i consumatori
	for i := 0; i < n_stage+1; i++ {
		channel[i]= make(chan int )
	}

	//partenza canali
	for i := 0; i < n_stage; i++ {
		// gli passo il canale corrente e il successivo
		fmt.Printf("creato il canale tra [%d]-[%d]\n", i,i+1)
		go server(i,channel[i],channel[i+1])
	}

	//partenza produttori
	for i := 0; i < max_cons; i++ {
		go produttore(i,channel[0])
	}

	//partenza consumatori
	for i := 0; i < max_cons; i++ {
		//leggono dall'ultimo canale
		fmt.Printf("consumatore [%d] legge da canale [%d]\n", i,n_stage)
		go consumatore(i,channel[n_stage])
	}

	for i := 0; i < max_prod; i++ {
		id_prod := <-done_prod
		fmt.Printf("terminato produttore [%d]\n", id_prod)
	}
	for i := 0; i < max_cons; i++ {
		id_cons := <-done_cons
		fmt.Printf("terminato consumatore[%d]\n",id_cons )
	}

	fmt.Printf("attesa della terminazione dei server\n")

	for i := 0; i < n_stage; i++ {
		end<-true
		id_chan := <-done_server
		fmt.Printf("server [%d] terminato\n", id_chan)
	}
	
}

