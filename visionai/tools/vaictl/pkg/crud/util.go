// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

package crud

import (
	"math/rand"
	"time"
)

func init() {
	rand.Seed(time.Now().UnixNano())
}

const defaultResourceIDLength = 8
const resourceIDFirstAlphabet = "abcdefghijklmnopqrstuvwxyz"
const resourceIDSuffixAlphabet = "0123456789abcdefghijklmnopqrstuvwxyz"

func randomString(length int, alphabet string) string {
	result := make([]byte, length)
	for i := range result {
		result[i] = alphabet[rand.Intn(len(alphabet))]
	}
	return string(result)
}

func randomResourceID(length int) string {
	return randomString(1, resourceIDFirstAlphabet) +
		randomString(length-1, resourceIDSuffixAlphabet)
}
