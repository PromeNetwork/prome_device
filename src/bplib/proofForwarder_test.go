package main

import (
	"fmt"
	"math/big"
	"testing"
)

func TestCheckTypeMissed(t *testing.T) {
	rawInput := ProverInput{4000, 10}
	mapping := map[int]RenderedStandard{2: {10000, 50500, 2}, 9: {10000, 69990, 1}}
	_, err := checkType(rawInput.ProverType, mapping)
	if err == nil {
		t.Error(errorTestProofForwarder("TestCheckTypeMissed", "error", "nil"))
	}
}

func TestCheckTypeFound(t *testing.T) {
	rawInput := ProverInput{4000, 9}
	mapping := map[int]RenderedStandard{2: {10000, 50500, 2}, 9: {10000, 69990, 1}}
	want := RenderedStandard{10000, 69990, 1}
	got, err := checkType(rawInput.ProverType, mapping)
	if err != nil || got != want {
		t.Error(errorTestProofForwarder("TestCheckTypeFound", want, got))
	}
}

func TestRawToMaterial(t *testing.T) {
	rawInput := ProverInput{4000, 9}
	mapping := map[int]RenderedStandard{2: {10000, 50500, 2}, 9: {10000, 69990, 1}}
	want := []int64{30000, 29990}
	got, err := rawToMaterial(rawInput, mapping)
	if err != nil || got[0].Cmp(big.NewInt(want[0])) != 0 || got[1].Cmp(big.NewInt(want[1])) != 0 {
		t.Error(errorTestProofForwarder("TestRawToMaterial", want, got))
	}
}

func errorTestProofForwarder(testCase string, wantElement interface{}, gotElement interface{}) string {
	return fmt.Sprintf("The test case is: %v\n Want: %v\n Got: %v\n", testCase, wantElement, gotElement)
}
