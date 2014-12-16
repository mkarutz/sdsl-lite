package main

import "os"
import "log"
import "path/filepath"
import "encoding/json"
import "os/exec"
import "time"
import "fmt"
import "strconv"
import "strings"

type TestFile struct {
	Name string `json:"name"`
	Url string  `json:"url"`
	NumBytes int `json:"numbytes"`
}

func main() {
	resFile := fmt.Sprintf("results/results-%s.csv",time.Now().Format(time.RFC3339))
	indexBins,err := filepath.Glob("./indexes/*.idx")
	log.Printf("Running benchmarks for indexes %v",indexBins)
	log.Printf("Storing results to file %v",resFile)

	// write result header
	rFile, err := os.Create(resFile)
	fmt.Fprintf(rFile,"file;type;sa_sample;isa_sample;metric;spaceusage;textsize;iterations;value;checksum\n")
	rFile.Close()

	testConfigFile,err := filepath.Glob("./config/files/*.json")
	if err != nil {
		log.Fatalf("Glob file error %v",err)
	}

	for _,fileDesc := range testConfigFile {
		jFile, err := os.Open(fileDesc)
		if err != nil {
			log.Fatal(err)
		}
		dec := json.NewDecoder(jFile)
		var testFileDesc TestFile
		err = dec.Decode(&testFileDesc)
		if err != nil {
			log.Fatalf("test config decoding error %v",err)
		}
		fileIndexBins,err := filepath.Glob("./indexes/"+testFileDesc.Name+"*.idx")
		for _,index := range fileIndexBins {
			indexType := strings.TrimPrefix(index,"indexes/"+testFileDesc.Name+"-")
			indexType = strings.TrimSuffix(indexType,".idx")
			log.Printf("Benchmarking index '%v' for file '%v'",indexType,testFileDesc.Name)
			benchCmd := "bin/measure-"+indexType+".x"
			numBytesStr := strconv.Itoa(testFileDesc.NumBytes)
			cmd := exec.Command(benchCmd,index,testFileDesc.Name,numBytesStr,resFile)
			cmd.Stdout = os.Stdout
			err := cmd.Run()
			if err != nil {
				log.Printf("Error benchmarking index: %v",err)
			}
			log.Printf("Done benchmarking index '%v' for file '%v'",indexType,testFileDesc.Name)
		}
	}
}
