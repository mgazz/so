package main

import (
	"fmt"
	"math/rand"
	"time"
)

const (
	MAXPROC= 10
	MAXBUFF= 10
	MAX = 4
	N=2
)

func when(b bool, c chan int) chan int {
	if !b {
		return nil
	}
	return c
}
//-------------------------------------


//canali
var done = make(chan bool)
var termina_gestore = make(chan bool)
var ack_cliente[MAXPROC] chan int
var req_prenotazione_cliente= make(chan int,MAXBUFF)
var req_ritiro_cliente= make(chan int,MAXBUFF)

var termina_sfoglina= make(chan bool)
var ack_sfoglina[MAXPROC] chan int 
var req_deposito_sfoglina= make(chan int,MAXBUFF) 

func cliente(id int) {
	fmt.Printf("cliente %d | avviato\n", id)
	req_prenotazione_cliente<-id
	ticket:=<-ack_cliente[id]
	if ticket == -1 {
		fmt.Printf("cliente %d | prenotazione non consentita: %d\n", id,ticket)
	} else {
		fmt.Printf("cliente %d | ricevuto ticket: %d\n", id,ticket)
		req_ritiro_cliente <- ticket
		result:= <-ack_cliente[id]
		if result==-1 {
			fmt.Printf("cliente %d | ticket non accettato: %d\n", id,ticket)
		} else {
			fmt.Printf("cliente %d | confezione ritirata con successo: %d\n", id,result)
		}
	}
	done <- true
	fmt.Printf("cliente %d | esce dal sistema\n", id)
}

func gestore() {
	var contatore int =0
	var confezioni_nel_frigo int =0
	//var log_ticket [MAX] int
	
	fmt.Printf("gestore # | avviato\n")
	for  {
		fmt.Printf("\t\treq_prenotazione_cliente:%d\n\t\treq_ritiro_cliente:%d\n\t\treq_deposito_sfoglina:%d\n\t\tconfezionie_nel_frigo:%d\n",len(req_prenotazione_cliente),len(req_ritiro_cliente),len(req_deposito_sfoglina),confezioni_nel_frigo)

		select {
			case x:= <-when(true,req_prenotazione_cliente):
				fmt.Printf("gestore # |%d entrato \n",x )
				if contatore < MAX {
					ack_cliente[x] <- contatore
					contatore++ 
				} else {
					ack_cliente[x] <- -1
				}
			case x:= <-when(confezioni_nel_frigo > 0 && len(req_prenotazione_cliente)==0,req_ritiro_cliente): //presenti confezione consegnabili
				confezioni_nel_frigo--
				ack_cliente[x] <- 1
				//for ticket := range log_ticket {
					//if x == ticket {
						//fmt.Printf("gestore # |ticket: %d usato in precedenza\n",x)
						//ack_cliente[x] <- -1
					//} else {
						//confezioni_nel_frigo--
						//ack_cliente[x] <- 1
					//}
				//}
			//NB: confezioni_new_fringo==0 necessario o deadlock su req_ritiro_cliente
			case x:= <-when((confezioni_nel_frigo < N && len(req_prenotazione_cliente)==0 && len(req_ritiro_cliente)==0) || confezioni_nel_frigo==0,req_deposito_sfoglina):
				fmt.Printf("deposito_sfoglina %d entrato \n",x )
				confezioni_nel_frigo++
				ack_sfoglina[x] <- 1
			
		case <-termina_gestore: //terminazione server
			fmt.Printf("gestore # | termina\n")
			done <- true
			return
		}
	}
}

func sfoglina(id int) {
	fmt.Printf("sfoglina %d | avviato\n", id)

	for  {
		select {
		case <-termina_sfoglina: //terminaziona sfoglina  
			fmt.Printf("sfoglina %d | termina\n",id)
			done <- true
			return
		default:
			time.Sleep(time.Duration(rand.Intn(10)) * time.Second)
			req_deposito_sfoglina<-id
			<-ack_sfoglina[id]
			fmt.Printf("sfoglina %d | depositata sfoglina\n",id)

		}
	}
}

func main() {

	var nclienti int //numero di client
	var nsfogline int 

	fmt.Printf("inserisci nclienti (max %d)? ", MAXPROC)
	fmt.Scanf("%d", &nclienti)

	fmt.Printf("inserisci nsfogline (max %d)? ", MAXPROC)
	fmt.Scanf("%d", &nsfogline)


	for i := 0; i < nsfogline; i++ { //canali ack per sfoglina
		//ack_sfoglina[i]= make(chan int,MAXBUFF) 
		ack_sfoglina[i]= make(chan int) 
	}

	for i := 0; i < nclienti; i++ { //canali ack per cliente
		//ack_cliente[i]= make(chan int,MAXBUFF) 
		ack_cliente[i]= make(chan int) 
	}

	for i := 0; i < nsfogline; i++ { //start sfoglina
		go sfoglina(i)
	}

	go gestore()

	for i := 0; i < nclienti; i++ {
		go cliente(i)
	}

	//aspetta client
	for i := 0; i < nclienti; i++ {
		<-done
	}
	fmt.Printf("main @ | terminati clienti\n")

	for i := 0; i < nsfogline; i++ { //metti prima terminazione server
		termina_sfoglina<- true
		<-done
	}
	fmt.Printf("main @ | terminati sfogline\n")

	//aspetta server
	termina_gestore <- true
	<-done
	fmt.Printf("FINE\n")
}

