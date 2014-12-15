package main

import "os"
import "log"
import "path/filepath"
import "os/exec"

type TestFile struct {
	Name string `json:"name"`
	Url string  `json:"url"`
	NumBytes int `json:"numbytes"`
}

func main() {
	resultFiles,err := filepath.Glob("./results/*.csv")
	if err != nil {
		log.Fatalf("Glob file error %v",err)
	}

	for _,resFile := range resultFiles {
		outFileName := resFile+".pdf"
		if _, err := os.Stat(outFileName); os.IsNotExist(err) {
			log.Printf("Visualizing results in file '%v'",resFile)
			cmd := exec.Command("Rscript","tools/visualize_results.R",resFile)
			cmd.Stdout = os.Stdout
			err := cmd.Run()
			if err != nil {
				log.Printf("Error visualizing results in file: %v",err)
			}
			log.Printf("Done visualizing results to file '%v'",outFileName)
		}
	}
}
