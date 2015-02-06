package main

import (
	"fmt"
	//"math/rand"
	//"time"
)

const (
	MAXPROC= 10
	MAXBUFF= 10
)

func when(b bool, c chan int) chan int {
	if !b {
		return nil
	}
	return c
}
//-------------------------------------

//canali


//canali
var done = make(chan bool)
var termina = make(chan bool)
var ack_cliente[MAXPROC] chan bool 
var req_cliente_prenotazione= make(chan int,MAXBUFF)//dif req per cliente->cambia nome e copia
var req_cliente_ritiro= make(chan int,MAXBUFF)//dif req per cliente->cambia nome e copia

//inserisci in main
	var nsfogline int //numero di client

	fmt.Printf("inserisci nsfogline (max %d)? ", MAXPROC)
	fmt.Scanf("%d", &nsfogline)


	for i := 0; i < nsfogline; i++ { //canali ack per sfoglina
		ack_sfoglina[i]= make(chan bool,MAXBUFF) 
	}
	for i := 0; i < nsfogline; i++ { //start sfoglina
		go sfoglina(i)
	}
	for i := 0; i < nsfogline; i++ { //aspetto sfoglina
		<-done
	}
//

var ack_sfoglina[MAXPROC] chan bool 
var req_sfoglina= make(chan int,MAXBUFF) //cambia nome e copia se diverse req per sfoglina
func sfoglina(id int) {
	fmt.Printf("sfoglina %d | avviato\n", id)

	done <- true
	fmt.Printf("sfoglina %d | esce dal sistema\n", id)
}

func cliente(id int) {
	fmt.Printf("cliente %d | avviato\n", id)

	done <- true
	fmt.Printf("cliente %d | esce dal sistema\n", id)
}

func gestore() {
	fmt.Printf("gestore # | avviato\n")
	for  {
		select {
			
		case <-termina: //terminazione server
			fmt.Printf("gestore # | termina\n")
			done <- true
			return
		}
	}
}

func main() {

	var nclienti int //numero di client

	fmt.Printf("inserisci nclienti (max %d)? ", MAXPROC)
	fmt.Scanf("%d", &nclienti)

	var nsfogline int //numero di client

	fmt.Printf("inserisci nsfogline (max %d)? ", MAXPROC)
	fmt.Scanf("%d", &nsfogline)


	for i := 0; i < nsfogline; i++ { //canali ack per sfoglina
		ack_sfoglina[i]= make(chan bool,MAXBUFF) 
	}
	for i := 0; i < nsfogline; i++ { //start sfoglina
		go sfoglina(i)
	}
	for i := 0; i < nsfogline; i++ { //aspetto sfoglina
		<-done
	}

	for i := 0; i < nclienti; i++ { //canali ack per cliente
		ack_cliente[i]= make(chan bool,MAXBUFF) 
	}
	go gestore()

	for i := 0; i < nclienti; i++ {
		go cliente(i)
	}

	//aspetta client
	for i := 0; i < nclienti; i++ {
		<-done
	}
	//aspetta server
	termina <- true
	<-done
	fmt.Printf("FINE\n")
}

