package main

import (
	"fmt"
	//"math/rand"
	//"time"
)

const (
	MAXBUFF= 10
	TI=4
	TS=5
	MAXPROC= TI+TS-1
	PS=0 //ponte superiore
	PI=1 //ponte inferiore
	I=0 //input
	O=1 //output
)

func when(b bool, c chan int) chan int {
	if !b {
		return nil
	}
	return c
}
//-------------------------------------





var done = make(chan bool)
var termina_server = make(chan bool)
var ack_cliente[MAXPROC] chan int
var req_cliente[2][2] chan int

func cliente(id int) {
	fmt.Printf("cliente %d | avviato\n", id)
	req_cliente[0][0]<-id
	<-ack_cliente[id]
	req_cliente[0][1]<-id
	<-ack_cliente[id]
	req_cliente[1][0]<-id
	<-ack_cliente[id]
	req_cliente[1][1]<-id
	<-ack_cliente[id]
	done <- true
	fmt.Printf("cliente %d | esce dal sistema\n", id)
}

func server() {
	fmt.Printf("server # | avviato\n")
	for  {
		//linea di stampa stato
		fmt.Printf("server # |\t") 

		fmt.Printf("\n")
		select {
		case x:= <-when(true,req_cliente[0][0]):
			ack_cliente[x] <- 1
		
		case x:= <-when(true,req_cliente[0][1]):
			ack_cliente[x] <- 1

		case x:= <-when(true,req_cliente[1][0]):
			ack_cliente[x] <- 1

		case x:= <-when(true,req_cliente[1][1]):
			ack_cliente[x] <- 1
			
		case <-termina_server: //terminazione server
			fmt.Printf("server # | termina\n")
			done <- true
			return
		}
	}
}
func init_chan() {
	for i := 0; i < 2; i++ {
		for j := 0; j < 2; j++ {
			req_cliente[i][j] = make(chan int,MAXBUFF) 
		}
	}
	for i := 0; i < nclient; i++ { //canali ack per cliente
		ack_cliente[i]= make(chan int,MAXBUFF) 
	}
}

var nclient int //numero di client

func main() {


	fmt.Printf("inserisci nclient (max %d)? ", MAXPROC)
	fmt.Scanf("%d", &nclient)

	init_chan()

	go server()

	for i := 0; i < nclient; i++ {
		go cliente(i)
	}

	//aspetta client
	for i := 0; i < nclient; i++ {
		<-done
	}

	//aspetta server
	termina_server <- true
	<-done
	fmt.Printf("FINE\n")
}

