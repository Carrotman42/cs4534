package main

import (
	"fmt"
	"io"
	"bytes"
	"image"
	"image/color"
	"image/png"
	"encoding/base64"
)

const Width = 1
type LineGraph struct {
	data []int
}
func (l*LineGraph) Append(data int) {
	l.data = append(l.data, data)
}
func (l LineGraph) Html(w io.Writer) error {
	var buf bytes.Buffer
	if err := png.Encode(&buf, l); err != nil {
		return err
	}
	
	fmt.Fprint(w, `<img src="data:image/png;base64,`)
	fmt.Fprint(w, base64.StdEncoding.EncodeToString(buf.Bytes()))
	fmt.Fprintf(w, `" /><!--%s-->`, l.data)
	return nil
}
func (LineGraph) ColorModel() color.Model {
	return color.RGBAModel
}
func (l LineGraph) Bounds() image.Rectangle {
	return image.Rect(0, 0, len(l.data)*Width, 255)
}

var (
	Red = color.RGBA{255, 0, 0, 255}
	Green = color.RGBA{0, 255, 0, 255}
	Blue = color.RGBA{255, 0, 255, 255}
	White = color.RGBA{255, 255, 255, 255}
	Black = color.RGBA{0, 0, 0, 255}
)

func (h LineGraph) At(x, y int) color.Color {
	y = 255-y
	pos, shi := x / Width, x % Width
	if Width > 3 && (shi == 0 || shi == 4) {
		return White
	}
	v := h.data[pos]
	if y == v || y + 1 == v || y - 1 == v {
		return Black
	}
	return White
}










