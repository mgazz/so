// Package main provides ...
package main

import (
	"fmt"
	"time"
)

//asincrono
//func main() {
	//ch := make(chan int ,50)
	//go func() {
		//time.Sleep(2*1e9)
		//x:=<-ch
		//fmt.Println("ricevuto: ",x)
	//}()
	//fmt.Println("sending",10)
	//ch<-10 //non sospensiva
	//fmt.Printf("inviato non aspetto risposta %d\n", 10)
	//time.Sleep(5*1e9)
	//fmt.Printf("end\n")
//}

//sincrono
func main() {
	ch := make(chan int )
	go func() {
		time.Sleep(2*1e9)
		x:=<-ch
		fmt.Println("ricevuto: ",x)
	}()
	fmt.Println("sending",10)
	ch<-10 //non sospensiva
	fmt.Printf("inviato non aspetto risposta %d\n", 10)
	time.Sleep(5*1e9)
	fmt.Printf("end\n")
}

