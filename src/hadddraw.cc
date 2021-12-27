#include <iostream>
#include <fstream>
#include <map>
#include <boost/filesystem.hpp>
#include <getopt.h>
#include <TSystem.h>
#include <TFile.h>
#include <TKey.h>
#include <TH1.h>
#include <TCanvas.h>
using namespace std;

// Parameters
int verb = 1;
string dir_name_out = "hadddraw_out";
int search_max_depth = 999;
string search_file_name = "";

// Outputs
ofstream fs_out;
TFile* file_out = 0;
map<string, TH1*> list_h1;

void InitOutput();
void AddFile(const string file_name);
void PrintHelp(std::ostream& os=std::cout)
{
  cout << "hadddraw\n"
       << "  Program to add and draw histograms in multiple ROOT files.\n"
       << "\n"
       << "Typical usage:\n"
       << "  hadddraw file_1.root file_2.root file_n.root\n"
       << "    All TH1-based histograms in the given ROOT files are merged\n"
       << "    and saved in 'hadddraw_out/hadddraw.root'.  They are also drawn\n"
       << "    as 'hadddraw_out/[histogram_name].png'  The output directory is\n"
       << "    first cleaned up if it exists.\n"
       << "\n"
       << "Options:\n"
       << "  -v     :  Increase the verbosity.\n"
       << "  -q     :  Decrease the verbosity (quiet).\n"
       << "  -o name:  Change the output directory from 'hadddraw_out/' to 'name/'.\n"
       << "  -h     :  Print this help message.\n"
       << "\n"
       << "Tips:\n"
       << "  You can use various shell methods to find and give ROOT files.\n"
       << "    hadddraw output/directory/*/data.root\n"
       << "    find output_dir -name data.root | xargs -n 1 hadddraw\n\n";
}

///
/// Main
///
int main(int argc, char** argv)
{
  int opt;
  while ((opt = getopt(argc, argv, "vqo:d:f:h")) != -1) {
    switch (opt) {
    case 'v':  verb++;  break;
    case 'q':  verb--;  break;
    case 'o':  dir_name_out = optarg;  break;
    case 'd':  search_max_depth = atoi(optarg);  break;
    case 'f':  search_file_name = optarg;  break;
    case 'h':  PrintHelp();  exit(0);  break;
    default:
      cout << "Invalid option.\n";
      PrintHelp();
      cout << "Abort.\n";
      exit(1);
    }
  }
  int n_input = argc - optind;
  if (n_input < 1) {
    cout << "No input file is given.  Abort.\n";
    exit(1);
  }

  gSystem->Unsetenv("LD_LIBRARY_PATH"); // To avoid loading libCling.so etc. from another ROOT lib.
  InitOutput();

  if (verb >= 1) {
    cout << "Adding histograms...\n"
         << "  N of input files = " << n_input << endl;
  }
  for (int ii = 1; ii <= n_input; ii++) {
    string file_name = argv[optind + ii - 1];
    if (verb >= 1) {
      cout << "  " << setw(3) << ii << ": " << file_name << endl;
    }
    if (file_name.length() >= 5 &&
        file_name.substr(file_name.length()-5, 5) == ".root") {
      AddFile(file_name);
    }
  }
  if (verb >= 1) {
    cout << "...done.\n\n"
         << "Drawing histograms..." << endl;
  }
  if (file_out) {
    gErrorIgnoreLevel = 1111;
    TCanvas* c1 = new TCanvas("c1", "");
    c1->SetGrid(true);
    for (map<string, TH1*>::iterator it = list_h1.begin(); it != list_h1.end(); it++) {
      string obj_name = it->first;
      TH1*   h1       = it->second;
      h1->Draw();
      string draw_name = dir_name_out + "/" + obj_name + ".png";
      if (verb >= 1) {
        cout << "  " << draw_name << endl;
      }
      c1->SaveAs(draw_name.c_str());
    }
    delete c1;

    file_out->Write();
    file_out->Close();
    delete file_out;

    fs_out.close();
  }
  if (verb >= 1) {
    cout << "...done.\n\n"
         << "E N D" << endl;
  }
  return 0;
}

///
/// Functions
///
void InitOutput()
{
  string fn_root = dir_name_out + "/hadddraw.root";
  string fn_txt  = dir_name_out + "/input_list.txt";
  if (verb >= 1) {
    cout << "Initialize the output files...\n"
         << "  " << fn_root << "\n"
         << "  " << fn_txt << "\n";
  }
  boost::filesystem::remove_all(dir_name_out.c_str());
  gSystem->mkdir(dir_name_out.c_str(), true);
  file_out = new TFile(fn_root.c_str(), "RECREATE");
  fs_out.open(fn_txt);
  if (verb >= 1) cout << "...done.\n" << endl;
}

void AddFile(const string file_name)
{
  static bool called_1st = true;

  TFile* file = new TFile(file_name.c_str());
  if (! file->IsOpen()) {
    delete file;
    return;
  }
  TIter next(file->GetListOfKeys());
  TKey* key;
  while ((key = (TKey*)next())) {
    string class_name = key->GetClassName();
    TObject* obj      = key->ReadObj();
    string obj_name   = obj->GetName();
    bool is_hist      = obj->InheritsFrom("TH1");
    if (! is_hist) continue;
    if (called_1st && verb >= 2) {
      cout << "      " << class_name << "  " << obj_name << endl;
    }

    if (list_h1.find(obj_name) == list_h1.end()) { // Create
      //if (! file_out) InitOutput();
      file_out->cd();
      list_h1[obj_name] = (TH1*)obj->Clone(obj_name.c_str());
    } else { // Just add
      list_h1[obj_name]->Add((TH1*)obj);
    }
  }

  fs_out << file_name << "\n";

  delete file;
  called_1st = false;
}
