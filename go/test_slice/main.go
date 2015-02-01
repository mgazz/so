// Package main provides ...
package main

import (
	"fmt"
	//"time"
)
const (
	n_res =3 
	
)
func main() {
	var coda_client []int
	var coda_ris_disp [] int

	//for i :=  range n_res{
		//coda_ris_disp=append(coda_ris_disp,i)
	//}
	for i := 0; i < n_res; i++ {
		coda_ris_disp=append(coda_ris_disp,i)
	}
	fmt.Printf("coda_ris_disp:%d\n", coda_ris_disp)
	fmt.Printf("coda_client len:%d\n", len(coda_client))
	coda_ris_disp = coda_ris_disp[1:]
	fmt.Printf("coda_ris_disp:%d\n", coda_ris_disp)
}

