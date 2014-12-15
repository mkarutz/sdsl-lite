package main

import "os"
import "log"
import "path/filepath"
import "encoding/json"
import "flag"
import "os/exec"
import "time"
import "fmt"
import "strconv"

type TestFile struct {
	Name string `json:"name"`
	Url string  `json:"url"`
	NumBytes int `json:"numbytes"`
}

func main() {
	flag.Parse()
	indexes := flag.Args()
	resFile := fmt.Sprintf("results/results-%s.csv",time.Now().Format(time.RFC3339))
	log.Printf("Running benchmarks for indexes %v",indexes)
	log.Printf("Storing results to file %v",resFile)

	// write result header
	rFile, err := os.Create(resFile)
	fmt.Fprintf(rFile,"file;type;class;sa_sample;isa_sample;metric;spaceusage;textsize;iterations;value;checksum\n")
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
		outFileName := "../data/"+testFileDesc.Name
		if _, err := os.Stat(outFileName); os.IsNotExist(err) {

		} else {
			for _,index := range indexes {
				log.Printf("Benchmarking index '%v' for file '%v'",index,testFileDesc.Name)
				indexFile := "indexes/"+testFileDesc.Name+"-"+index
				if _, err := os.Stat(indexFile); os.IsNotExist(err) {
				} else {
					benchCmd := "bin/measure-"+index
					numBytesStr := strconv.Itoa(testFileDesc.NumBytes)
					cmd := exec.Command(benchCmd,indexFile,testFileDesc.Name,numBytesStr,resFile)
					cmd.Stdout = os.Stdout
					err := cmd.Run()
					if err != nil {
						log.Printf("Error benchmarking index: %v",err)
					}
					log.Printf("Done benchmarking index '%v'",indexFile)
				}
			}

		}
	}
}
