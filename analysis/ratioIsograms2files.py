import argparse
import ROOT

def main():
    # Argument parser setup
    parser = argparse.ArgumentParser(description="Plot ratio of two histograms from two ROOT files")
    parser.add_argument("file1", help="First ROOT file")
    parser.add_argument("file2", help="Second ROOT file")
    parser.add_argument("hist_name1", help="Histogram name in first file")
    parser.add_argument("--hist_name2", help="Histogram name in second file (defaults to same as first)", default=None)
    parser.add_argument("--xlog", action="store_true", help="Set x-axis to log scale")
    parser.add_argument("--ylog", action="store_true", help="Set y-axis to log scale")
    args = parser.parse_args()

    # Use hist_name1 for second histogram if hist_name2 not provided
    hist_name2 = args.hist_name2 if args.hist_name2 else args.hist_name1

    # Open ROOT files
    file1 = ROOT.TFile.Open(args.file1)
    file2 = ROOT.TFile.Open(args.file2)

    if not file1 or file1.IsZombie():
        print(f"Failed to open file: {args.file1}")
        return
    if not file2 or file2.IsZombie():
        print(f"Failed to open file: {args.file2}")
        file1.Close()
        return

    # Retrieve histograms
    hist1 = file1.Get(args.hist_name1)
    hist2 = file2.Get(hist_name2)

    if not hist1:
        print(f"Histogram {args.hist_name1} not found in file: {args.file1}")
        file1.Close()
        file2.Close()
        return
    if not hist2:
        print(f"Histogram {hist_name2} not found in file: {args.file2}")
        file1.Close()
        file2.Close()
        return

    # Clone and calculate ratio
    ratio_hist = hist1.Clone("ratio_hist")
    ratio_hist.Divide(hist2)
    ratio_hist.SetTitle(f"Ratio of {args.hist_name1} (file1) to {hist_name2} (file2)")
    ratio_hist.GetYaxis().SetTitle("Ratio")
    ratio_hist.GetXaxis().SetTitle(hist1.GetXaxis().GetTitle())

    # Save plotted data to text file
    with open("transmission.txt", "w") as f:
        f.write("# x y\n")
        for i in range(1, ratio_hist.GetNbinsX() + 1):
            x = ratio_hist.GetBinCenter(i)
            y = ratio_hist.GetBinContent(i)
            f.write(f"{x} {y}\n")

    # Canvas setup
    canvas = ROOT.TCanvas("canvas", "Canvas", 800, 600)
    if args.xlog:
        canvas.SetLogx()
    if args.ylog:
        canvas.SetLogy()

    # Draw ratio plot
    ratio_hist.Draw()
    canvas.SaveAs("RatioPlotOutput.png")

    # Close files
    file1.Close()
    file2.Close()

if __name__ == "__main__":
    main()
