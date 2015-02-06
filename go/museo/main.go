package main

import (
	"fmt"
	"math/rand"
	"time"
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

//----------------------------------------------

//canali
var done = make(chan bool)
var termina = make(chan bool)
var uscita_visitatore= make(chan int) //se entra ed esce dal sistema ==> non usare se si stratta solo di servire il thread
var uscita_visitatore_museo= make(chan int) 
var uscita_visitatore_mostra= make(chan int) 
var uscita_visitatore_entrambe= make(chan int) 

var ack_visitatore[MAXPROC] chan bool 
var req_visitatore_museo= make(chan int,MAXBUFF)
var req_visitatore_mostra= make(chan int,MAXBUFF)
var req_visitatore_entrambe= make(chan int,MAXBUFF)

var termina_operatore= make(chan bool)
var ack_operatore[MAXPROC] chan bool 
var req_operatore_entrata= make(chan int,MAXBUFF) //cambia nome e copia se diverse req per operatore
var req_operatore_uscita= make(chan int,MAXBUFF) //cambia nome e copia se diverse req per operatore

func visitatore(id int) {
	fmt.Printf("visitatore %d | avviato\n", id)
	var tipo_vis int
	tipo_vis = rand.Intn(3)
	fmt.Printf("visitatore %d | tipo: %d\n", id,tipo_vis)

	if tipo_vis==0 { //tipo 1 : museo
		req_visitatore_museo<-id
		<-ack_visitatore[id]
		time.Sleep(time.Duration(rand.Intn(3)) * time.Second)
		uscita_visitatore_museo<-id
		<-ack_visitatore[id]
	} 
	if tipo_vis==1 { //tipo 2 : mostra
		req_visitatore_mostra<-id
		<-ack_visitatore[id]
		time.Sleep(time.Duration(rand.Intn(3)) * time.Second)
		uscita_visitatore_mostra<-id
		<-ack_visitatore[id]
	} 
	if tipo_vis==3-1 {//tipo 3: museo e mostra
		req_visitatore_entrambe<-id
		<-ack_visitatore[id]
		time.Sleep(time.Duration(rand.Intn(3)) * time.Second)
		uscita_visitatore_entrambe<-id
		<-ack_visitatore[id]
	}

	done <- true
	fmt.Printf("visitatore %d | esce dal sistema\n", id)
}

func operatore(id int) {
	fmt.Printf("operatore %d | avviato\n", id)

	for  {
		select {
		case <-termina_operatore: //terminaziona operatore  
			fmt.Printf("operatore %d | termina\n",id)
			done <- true
			return
		default:
			req_operatore_entrata<-id
			<-ack_operatore[id]
			fmt.Printf("operatore %d | entrato in servizio\n",id)
			time.Sleep(time.Duration(rand.Intn(3)) * time.Second)
			req_operatore_uscita<-id
			<-ack_operatore[id]
			fmt.Printf("operatore %d | uscito dal servizio\n",id)
		}
	}
}

func museo() {
	fmt.Printf("museo # | avviato\n")
	var noperatori int
	var nmuseo int
	var nmostra int
	var nentrambe int
	for  {
		select {
		case x:= <-when(true,req_visitatore_museo):
			fmt.Printf("visitatore %d entrato museo\n",x )
			nmuseo++
			fmt.Printf("museo # | nmuseo: %d, nmostra:%d, nentrambe: %d , noperatori: %d\n",nmuseo,nmostra,nentrambe,noperatori)
			ack_visitatore[x] <- true

		case x:= <-when(true,req_visitatore_mostra):
			fmt.Printf("visitatore %d entrato mostra\n",x )
			nmostra++
			fmt.Printf("museo # | nmuseo: %d, nmostra:%d, nentrambe: %d , noperatori: %d\n",nmuseo,nmostra,nentrambe,noperatori)
			ack_visitatore[x] <- true

		case x:= <-when(true,req_visitatore_entrambe):
			fmt.Printf("visitatore %d entrato entrambe\n",x )
			nentrambe++
			fmt.Printf("museo # | nmuseo: %d, nmostra:%d, nentrambe: %d , noperatori: %d\n",nmuseo,nmostra,nentrambe,noperatori)
			ack_visitatore[x] <- true

		case x:= <-when(true,uscita_visitatore_museo):
			fmt.Printf("visitatore %d uscito museo\n",x )
			nmuseo--
			fmt.Printf("museo # | nmuseo: %d, nmostra:%d, nentrambe: %d , noperatori: %d\n",nmuseo,nmostra,nentrambe,noperatori)
			ack_visitatore[x] <- true

		case x:= <-when(true,uscita_visitatore_mostra):
			fmt.Printf("visitatore %d uscito mostra\n",x )
			nmostra--
			fmt.Printf("museo # | nmuseo: %d, nmostra:%d, nentrambe: %d , noperatori: %d\n",nmuseo,nmostra,nentrambe,noperatori)
			ack_visitatore[x] <- true

		case x:= <-when(true,uscita_visitatore_entrambe):
			fmt.Printf("visitatore %d uscito entrambe\n",x )
			nentrambe--
			fmt.Printf("museo # | nmuseo: %d, nmostra:%d, nentrambe: %d , noperatori: %d\n",nmuseo,nmostra,nentrambe,noperatori)
			ack_visitatore[x] <- true
		case x:= <-when(true,req_operatore_entrata):
			fmt.Printf("museo # | operatore %d entrato \n",x )
			noperatori++
			fmt.Printf("museo # | nmuseo: %d, nmostra:%d, nentrambe: %d , noperatori: %d\n",nmuseo,nmostra,nentrambe,noperatori)
			ack_operatore[x] <- true
		
		case x:= <-when(true,req_operatore_uscita):
			fmt.Printf("museo # | operatore %d uscito\n",x )
			noperatori--
			fmt.Printf("museo # | nmuseo: %d, nmostra:%d, nentrambe: %d , noperatori: %d\n",nmuseo,nmostra,nentrambe,noperatori)
			ack_operatore[x] <- true

		case <-termina: //terminazione server
			fmt.Printf("museo # | termina\n")
			done <- true
			return
		}
	}
}

func main() {

	var nclienti int //numero di client

	fmt.Printf("inserisci nclienti (max %d)? ", MAXPROC)
	fmt.Scanf("%d", &nclienti)


//inserisci in main
	var noperatori int //numero di client

	fmt.Printf("inserisci noperatori (max %d)? ", MAXPROC)
	fmt.Scanf("%d", &noperatori)


	for i := 0; i < noperatori; i++ { //canali ack per operatore
		ack_operatore[i]= make(chan bool,MAXBUFF) 
	}

	for i := 0; i < nclienti; i++ { //canali ack per visitatore
		ack_visitatore[i]= make(chan bool,MAXBUFF) 
	}

	go museo()

	for i := 0; i < noperatori; i++ { //start operatore
		go operatore(i)
	}

	for i := 0; i < nclienti; i++ { //start visitatore
		go visitatore(i)
	}

	//aspetta client
	for i := 0; i < nclienti; i++ {
		<-done
	}
	fmt.Printf("tutti i clienti sono usciti --> faccio terminare gli operatori\n")
	for i := 0; i < noperatori; i++ { //aspetto operatore circolare
		termina_operatore <- true
		<-done
	}
	//aspetta server
	termina <- true
	<-done

	fmt.Printf("FINE\n")
}

