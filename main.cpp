/*
 * =====================================================================================
 *
 *       Filename:  main.cpp
 *
 *    Description:  generator for model tests
 *
 *        Version:  1.0
 *        Created:  18.12.2015 23:49:12
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#include	<iostream>
#include  <fstream>
#include  <vector>
#include  <string>
#include  "modelgen.h"

using namespace std;

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  main
 *  Description:  
 * =====================================================================================
 */
	int
main ()
{
	// read();
	//  variables 
	//  in
	
  string line;
  vector<string> inputs;
  ifstream varsFile ("vars.txt");
  if (varsFile.is_open())
  {
    while ( getline (varsFile, line) )
    {
			inputs.push_back(line);
    }
    varsFile.close();
  }
  else 
	{
		cout << "Unable to open file\n"; 
	}

	//   out
  vector<string> outputs;
  ifstream outputsFile ("outs.txt");
  if (outputsFile.is_open())
  {
    while ( getline (outputsFile, line) )
    {
			outputs.push_back(line);
    }
    outputsFile.close();
  }
  else 
	{
		cout << "Unable to open file\n"; 
	}

	// read from genmap
	unsigned int inCount = inputs.size();
  vector<string> inTypes(inCount), inMin(inCount), inMax(inCount), inResolutions(inCount);
	vector<vector<string> > inFullNames(inCount);
	for (unsigned int i = 0; i < inCount; i++)
	{
  	ifstream genmap ("GVII_MWF_Map_GenMap.tsf");
  	if (genmap.is_open())
    {
    	while ( getline (genmap, line) )
    	{
	      // read fullname
			  if ( line.find("Dim " + inputs[i] + "_") != string::npos) 
			  {
			  	inFullNames[i].push_back(line.substr(4));
			  }

	      // read type
			  else if ( (!inFullNames[i].empty()) && 
						 (line.find(inFullNames[i][0]) == 0) ) 
			  {
					int p;
			    if ( line.find("ParameterGroup") != string::npos) 
            inTypes[i] = "PGS";
			    else if ( line.find("DiscreteStatus") != string::npos) 
            inTypes[i] = "DSSM";
			    else if ( line.find("NumericStatus") != string::npos) 
            inTypes[i] = "NSSM";
			    else if ( (p = line.find("Float")) != string::npos) 
					{
            inTypes[i] = "Float";

	          // read range from genmap
						int q = line.find(';');
						p += 6; // Float, = 6
						string t = line.substr(p, q - p);
						int m = t.find(',');
						int n = t.find(',', m + 1);
						inMin[i] = t.substr(0, m);
						inMax[i] = t.substr(m + 1, n - m - 1);

	          // read resolution from genmap
						inResolutions[i] = t.substr(n + 1);
					}
			    else if ( line.find("Enumeration") != string::npos) 
            inTypes[i] = "Enum";
			    else if ( line.find("Unsigned") != string::npos) 
            inTypes[i] = "uint";
			    else if ( line.find("Signed") != string::npos) 
            inTypes[i] = "int";
			    else if ( line.find("Text") != string::npos) 
            inTypes[i] = "Text";
			    else if ( line.find("FlagStatus") != string::npos) 
            inTypes[i] = "FlagStatus";
			    else if ( line.find("Bitfield") != string::npos) 
            inTypes[i] = "Bitfield";
			    else if ( line.find("Binary") != string::npos) 
            inTypes[i] = "Binary";
					else
            inTypes[i] = "Unknown";
			  }
			}
    }
    genmap.close();

		// print result of reading
		//cout <<  endl;
		//cout << "=======================" << endl;
		//cout << inputs[i] <<  endl;
		//cout << "-----------------------" << endl;
		//cout << inTypes[i] <<  endl;
		//cout << "-----------------------" << endl;
		//for (unsigned int j = 0; j < inFullNames[i].size(); j += 1 ) {
		//	cout << inFullNames[i][j] << endl;
		//}
  }
	
	//  read logic blocks
	//  block names
	vector<string> blocks;
	string systemName("LocalizerComparison");
	string modelName = systemName + '/';
  ifstream mdlFile ("misCompMon.mdl");
  if (mdlFile.is_open())
  {
    while ( getline (mdlFile, line) )
    {
			int p = -1;
			if ( (p = line.find(modelName)) != string::npos)
			{
				int m = line.find('/', p) + 1;
				int n = line.find('/', m);
				blocks.push_back(line.substr(m, n - m));
				cout << blocks.back() << '=' <<  endl;
			}
    }
    mdlFile.close();
  }

	// read ports count for blocks
	vector<string> portCount(blocks.size());
	ifstream mdlFile2("misCompMon.mdl");
  if (mdlFile2.is_open())
  {
		// seek to our model
		while ((line.find("Name") == string::npos) || 
				   (line.find(systemName) == string::npos))
		{
			getline(mdlFile2, line);
		}

		for (unsigned int j = 0; j < blocks.size(); j += 1 ) {
			if ((blocks[j].find("and") == string::npos) &&
					(blocks[j].find("or")  == string::npos) &&
					(blocks[j].find("sum") == string::npos) )
				continue;
			cout << blocks[j] << ' ';

			// seek to next block
		  while ((line.find("Name") == string::npos) || 
		  		   (line.find(blocks[j]) == string::npos))
			{
		  	getline(mdlFile2, line);
				if (mdlFile2.bad())
				{
					cout << "break" << endl;
					break;
				}
			}
		  getline(mdlFile2, line);
			int p = line.find('[') + 1;
			int q = line.find(',');
			portCount[j] = line.substr(p, q - p);
			cout << portCount[j] << " ports ";

			// read sign for Sum ports
			if (blocks[j].find("sum") != string::npos)
			{
				while ((line.find("MaskValueString") == string::npos) )
				{
					getline(mdlFile2, line);
				}
				p = line.find('"');
				q = line.find('"', ++p);
				cout << line.substr(p, q - p);
			}
			
			cout <<  endl;
		}

    mdlFile2.close();
  }
	
	// create steps();
	//  generate steps
	//  print table
	//  print steps
	cout << "---------------------\n";
	cout << "New test script file.\n";
	cout << "---------------------\n";
	return 0;
}				/* ----------  end of function main  ---------- */
