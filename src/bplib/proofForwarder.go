package main

import (
	"errors"
	"math"
	"math/big"
)

type ProverInput struct {
	ProverData float64 `json:"ProverData"`
	ProverType int     `json:"ProverType"`
}

func checkType(proverType int, rs map[int]RenderedStandard) (RenderedStandard, error) {
	var eurekaStandard RenderedStandard
	for key, value := range rs {
		if proverType == key {
			eurekaStandard = value
			return eurekaStandard, nil
		}
	}
	err := errors.New("there is no match proof type id, please check your proof type")
	return RenderedStandard{}, err
}

func rawTrimmer(r float64, p int) (m int) {
	var x float64
	if p >= 0 {
		x = math.Pow10(p)
	}
	m = int(math.Trunc(x * r))
	return
}

func rawToMaterial(raw ProverInput, rs map[int]RenderedStandard) (material []*big.Int, err error) {
	standardSelected, err := checkType(raw.ProverType, rs)
	if err != nil {
		return []*big.Int{}, err
	}
	m := rawTrimmer(raw.ProverData, standardSelected.precision)
	material = make([]*big.Int, 2)
	material[0] = big.NewInt(int64(m - standardSelected.low))
	material[1] = big.NewInt(int64(standardSelected.high - m))
	return material, nil
}
