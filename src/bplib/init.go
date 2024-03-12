
package main

/*
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
*/
import "C"

import (
	"bplib/bulletproof"
	"fmt"
	"unsafe"
)

const ProofUser = "WXblockchain"

func initMap(JsonFile string, m map[int]RenderedStandard) error {
	var reader []Standard
	err := getJson(JsonFile, &reader)
	if err != nil {
		return err
	}
	err = StandardPreparing(reader, m)
	if err != nil {
		return err
	}
	return nil
}

//export BulletProof
func BulletProof(JsonFile *C.char, rawData C.double, proofType C.int, proof **C.char) (errorCode int) {
	var raw ProverInput
	var aggBpParam *bulletproof.AggBpStatement
	var mapping map[int]RenderedStandard

	mapping = make(map[int]RenderedStandard)

	jf := C.GoString(JsonFile)
	err := initMap(jf, mapping)
	if err != nil {
		fmt.Println(err.Error());
		return 1
	}
	
	aggBpParam = bulletproof.GenerateAggBpStatement(2, 16)
	raw.ProverData = float64(rawData)
	raw.ProverType = int(proofType)
	v, err := rawToMaterial(raw, mapping)
	if err != nil {
		fmt.Println(err.Error())
		return 2
	}
	p, err := bulletproof.AggBpProve_s(aggBpParam, v)
	if err != nil {
		fmt.Println(err.Error())
		return 3
	}
	tmp  := C.CString(p)
	C.strcpy(*proof, (*C.char)(unsafe.Pointer( tmp )))
	defer C.free(unsafe.Pointer( tmp )) 
	return 0
}

func goBulletProof(JsonFile string, rawData float64, proofType int, proof *string, errorStr error) (errorCode int) {
	var raw ProverInput
	var aggBpParam *bulletproof.AggBpStatement
	var mapping map[int]RenderedStandard

	mapping = make(map[int]RenderedStandard)

	err := initMap(JsonFile, mapping)
	if err != nil {
		errorStr = err
		return 1
	}
	
	aggBpParam = bulletproof.GenerateAggBpStatement(2, 16)
	
	raw.ProverData = rawData
	raw.ProverType = proofType
	
	v, err := rawToMaterial(raw, mapping)
	if err != nil {
		errorStr = err
		return 2
	}
	*proof, err = bulletproof.AggBpProve_s(aggBpParam, v)
	if err != nil {
		errorStr = err
		return 3
	}
	return 0
}

func main() {
	var errString error
	var proof string
	proofType := 1
	rawData := 10.26

	errorCode := goBulletProof("standard2.json", rawData, proofType, &proof, errString)
	if errorCode != 0 {
	}
	println(proof)
}
