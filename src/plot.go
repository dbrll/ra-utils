package main

import (
	"bufio"
	"flag"
	"fmt"
	"gonum.org/v1/plot"
	"gonum.org/v1/plot/plotter"
	"gonum.org/v1/plot/vg"
	"image/color"
	"os"
	"strconv"
	"strings"
)

func main() {

	var title string
	var xlegend string
	var ylegend string
	var outputfile string

	flag.StringVar(&title, "title", "HI Spectrum", "Plot title")
	flag.StringVar(&xlegend, "xlegend", "Radial velocity (km/s)", "X legend")
	flag.StringVar(&ylegend, "ylegend", "Power (uncalibrated)", "Y legend")
	flag.StringVar(&outputfile, "output", "plot.png", "Output file")
	flag.Parse()

/*	file, err := ioutil.ReadFile(flag.Args())
    if err != nil {
        fmt.Println("Can't read file:", os.Args[1])
       	panic(err)
    }*/
    filename := strings.Join(flag.Args()," ")
    if len(filename) == 0  {
    	fmt.Println("Missing input file, exiting\n")
    	os.Exit(0)
    }

    file, err := os.Open(filename)
    if err != nil {
        fmt.Println("Cannot read input file")
        os.Exit(0)
    }
    defer file.Close()

	vel := []float64{}
	pwr := []float64{}
 
    scanner := bufio.NewScanner(file)
    for scanner.Scan() {
		val := strings.Split(scanner.Text(), " ")
		_vel, _ := strconv.ParseFloat(val[0], 64)
		_pwr, _ := strconv.ParseFloat(val[1], 64)
		vel = append(vel, _vel)
		pwr = append(pwr, _pwr)
    }
 
    if err := scanner.Err(); err != nil {
        fmt.Println(err)
    }

	// Find min and max pwr (Y) values
	minpwr := 127.0
	maxpwr := 0.0
	for i := range pwr {
		if pwr[i] < minpwr {
			minpwr = pwr[i]
		}
		if pwr[i] > maxpwr {
			maxpwr = pwr[i]
		}
	}

	// Get min and max X values
	minx := vel[0]
	maxx := vel[len(vel)-1]

	p := plot.New()
	if p == nil {
		panic(p)
	}

	p.Title.Text = title
	p.X.Label.Text = xlegend
	p.Y.Label.Text = ylegend

	// Create plot array
	pts := make(plotter.XYs, len(vel))
	for i := range pts {
		pts[i].X = vel[i]
		pts[i].Y = pwr[i]
	}

	line, err := plotter.NewLine(pts)
	if err != nil {
		panic(err)
	}
	line.Color = color.RGBA{B: 255, A: 255}
	line.Width = 0.5
	
	p.Add(line)

	p.X.Min = minx
	p.X.Max = maxx
	p.Y.Min = minpwr * 0.9
	p.Y.Max = maxpwr * 1.1

	p.Add(plotter.NewGrid())

	// Save the plot to a file.
	if err := p.Save(8*vg.Inch, 8*vg.Inch, outputfile); err != nil {
		panic(err)
	}
}
