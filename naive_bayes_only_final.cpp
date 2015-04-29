#include<iostream>
#include <fstream>
#include <map>
//#include <conio.h>
#include <time.h>
#include <vector>
#include <string.h>
#include<stdlib.h>
#include<math.h>
#define PI 3.14159265359
#define E 2.71828182846
#define sqrt_pi 2.506628274631

/* Author - Jayesh Kawli */

using namespace std;
int main()
{
    int total_time=0;
    map<string,int> values_ok;
    map<string,int> values_fraud;
    int temp=0;
    int vec_pos=0;
    int vec_overall=0;
    char buffer[40];
    string string1;
    map<int,long> tempholder;
    vector< vector<string> > sales_data_vec;
    int count=0;
    int uni=0;
    ofstream getoutdata1,getoutdata,getoutdatatrain;
    
    /*File for storing inferred labels for unknown data */
    
    getoutdata.open("TextFiles/fr.txt");
    
    /* Output file with all records including ok, fraud and unknowns inferred */
    
    getoutdata1.open("TextFiles/allokfraud.txt");

    /* File which stores all ok and fraud data from given pool of records.
    Basically contains training records. Which will then be used for PPV
    calculation */
    
    getoutdatatrain.open("TextFiles/okfraudmod.txt");
    
    
    /* Input file containing all records including ok, unknown and frauds and NA s */
    
    ifstream salesdata("TextFiles/salesf_transform1.txt");

    int gt=0;
    int indi;
    
    /* Read the sales data file line by line. Until it reaches end of records */
    
    while(!salesdata.eof())
    {
        indi=0;
        bool isunknown=false;
        vector<int> attributeholder;
        char *indiattri;
        salesdata.getline(buffer,40);
        char *pch = strtok (buffer,"\t");
        vec_pos=0;
        vector<string> temp;
        temp.clear();
        int test=0;
        while(pch!=NULL)
        {
            char temp12[10];
            strcpy(temp12,pch);
            temp.insert(temp.begin()+vec_pos,temp12);
            test++;
            if(!strcmp(pch,"NA"))
            {
                gt++;
                pch=0;
                indi=1;
                break;
            }
            vec_pos++;
            
             /* Records are delimited by tab space */
            
            pch=strtok(NULL,"\t");
        }
        
         /* Check to ignore records with missing values */
         
        if(indi==0)
        {
            sales_data_vec.insert(sales_data_vec.begin()+vec_overall,temp);
            vec_overall++;
        }
        else
        {
            indi=0;

        }
    }

    vector< vector<string> >::iterator it1;
    vector<string>::iterator it2;

    int number_ok=0,number_fraud=0,number_unknown=0;
    int quantity_fraud=0,quantity_ok=0,value_ok=0,value_fraud=0;
    
    /* Store the count for each value in attribute column 
    salesman and product id for both class labels ok and fraud
    Store the count. And this will further be used to calculate 
    probabilites which will be used to find labels for test data  
    */
    
    
    for(it1=sales_data_vec.begin(); it1!=sales_data_vec.end(); it1++)
    {
                                    
        /* If the record label is ok, then add values of quanity and values
        in appropriate variables this will then be used to calculate normalized
        value for each numerical attributes */                                                                        
                                    
        if(!(((*it1).at(4).compare("ok"))))
        {
            quantity_ok+=atoi((*it1).at(2).c_str());
            value_ok+=atoi((*it1).at(3).c_str());
            number_ok++;
        }
        
        /* If the record label is fraud, then add values of quanity and values
        in appropriate variables this will then be used to calculate normalized
        value for each numerical attributes */
        
        else if(!((((*it1).at(4)).compare("fraud"))))
        {
            quantity_fraud+=atoi((*it1).at(2).c_str());
            value_fraud+=atoi((*it1).at(3).c_str());
            number_fraud++;
        }
        
        /* Unknown Labels. Do nothing */
        
        else
        {
            number_unknown++;
        }
    }
cout<<endl<<endl;
    cout<<"Number of Unknowns   "<<number_unknown<<endl;
    cout<<"Number of Oks    "<<number_ok<<endl;
    cout<<"Number of Frauds   "<<number_fraud<<endl<<endl;
    cout<<"*************************************"<<endl;
    //cout<<quantity_fraud/number_fraud<<"ohh  "<<quantity_ok/number_ok<<"  "<<value_ok/number_ok<<"  "<<value_fraud/number_fraud<<endl;
    
    /* Calculate mean values in each of the quantity and values columns for
    both ok and fraud labels. This is to calculate normalized values of 
    numerical attributes and evaluate their probabilities for test/unknown data */
    
    double mean_quantity_fraud=(quantity_fraud/number_fraud);
    double mean_quantity_ok=(quantity_ok/number_ok);
    double mean_value_ok=(value_ok/number_ok);
    double mean_value_fraud=(value_fraud/number_fraud);

    double var_quantity_fraud=0,var_quantity_ok=0,var_value_ok=0,var_value_fraud=0;
    int flag=0;
    
    /* Once caluclate mean values, calculate variance using standard variance formula */
    
    for(it1=sales_data_vec.begin(); it1!=sales_data_vec.end(); it1++)
    {

        if(!(((*it1).at(4).compare("ok"))))
        {
            flag=1;
            var_quantity_ok+=pow((atoi((*it1).at(2).c_str())-mean_quantity_ok),2);
            var_value_ok+=pow((atoi((*it1).at(3).c_str())-mean_value_ok),2);
        }
        else if(!((((*it1).at(4)).compare("fraud"))))
        {
            flag=1;
            var_quantity_fraud+=pow((atoi((*it1).at(2).c_str())-mean_quantity_fraud),2);
            var_value_fraud+=pow((atoi((*it1).at(3).c_str())-mean_value_fraud),2);
        }
    }
    
    /* Divide by number of samples n. It might be n or (n-1) */
    
    var_quantity_fraud=(double)(var_quantity_fraud/number_fraud);
    var_quantity_ok=(double)(var_quantity_ok/number_ok);
    var_value_ok=(double)(var_value_ok/number_ok);
    var_value_fraud=(double)(var_value_fraud/number_fraud);
    
    /* Calculate standard deviation */
    
    double std_quantity_fraud=sqrt(var_quantity_fraud);
    double std_quantity_ok=sqrt(var_quantity_ok);
    double std_value_ok=sqrt(var_value_ok);
    double std_value_fraud=sqrt(var_value_fraud);

    double const1_quantity_fraud=(double)(1/(sqrt_pi*std_quantity_fraud));
    double const1_quantity_ok=(double)(1/(sqrt_pi*std_quantity_ok));
    double const1_value_ok=(double)(1/(sqrt_pi*std_value_ok));
    double const1_value_fraud=(double)(1/(sqrt_pi*std_value_fraud));

    double const2_quantity_fraud=(double)pow(E,-(double)(1/(2*var_quantity_fraud)));
    double const2_quantity_ok=(double)pow(E,-(double)(1/(2*var_quantity_ok)));
    double const2_value_ok=(double)pow(E,-(double)(1/(2*var_value_ok)));
    double const2_value_fraud=(double)pow(E,-(double)(1/(2*var_value_fraud)));
    
    /* Store all the records without missing values having
    labels as ok or fraud in another file.
    This file will then be used to calculate PPV values and
    estimate accuracy of Naive Bayes classifier model */
    
    for(it1=sales_data_vec.begin(); it1!=sales_data_vec.end(); it1++)
    {
        int y=0;

        if(!(*it1).at(4).compare("fraud") || !(*it1).at(4).compare("ok"))
        {
            while(y!=5)
            {
                /*                                if(y==0 || y==1)
                                                {
                                                  getoutdatatrain<<"1"<<"\t";
                                                getoutdata1<<"1"<<"\t";

                                                }
                                                else*/
                //   {
				if(y==4)
				break;
                getoutdatatrain<<(*it1).at(y)<<"\t";
                getoutdata1<<(*it1).at(y)<<"\t";
                // }
                y++;
            }
			double ratio11=(double)(atoi((*it1).at(3).c_str())/atoi((*it1).at(2).c_str()));
			getoutdatatrain<<ratio11<<"\t";
			getoutdatatrain<<(*it1).at(4)<<"\t";
            getoutdatatrain<<endl;
            getoutdata1<<endl;
            continue;
        }

        for(int i=0; i<2; i++)
        {
            if(!((*it1).at(4).compare("unkn")))
            {
                break;
            }
            else if(!((*it1).at(4).compare("fraud")))
            {

                getoutdata1<<endl;
                if(values_fraud.count((*it1).at(i))>0)
                {
                    values_fraud[(*it1).at(i)]++;
                }
                else
                {
                    values_fraud[(*it1).at(i)]=1;
                }
            }
            else if(!((*it1).at(4).compare("ok")))
            {
                if(values_ok.count((*it1).at(i))>0)
                {
                    values_ok[(*it1).at(i)]++;
                }
                else
                {
                    values_ok[((*it1).at(i))]=1;
                }
            }
        }
    }

    map<string,int>::iterator it3;
    getoutdatatrain.close();
    double temp_value_fraud=0,temp_quantity_fraud=0,temp_value_ok=0,temp_quantity_ok=0;
    double prior_estimate=0.5;
    double sample_size=5;
    int tot_training=number_ok+number_fraud;
    int test_ok=0,test_fraud=0,equal=0;
    
    /* Tested with different values of prior estimate. There was little difference 
    in final estimation of class labels for unknown data */
    
    double prior_estimate_fraud=0.5,prior_estimate_ok=0.5;
    
    /* Start the clock once execution begins */
    
    total_time=clock();
    
    for(it1=sales_data_vec.begin(); it1!=sales_data_vec.end(); it1++)
    {
        if(!((*it1).at(4).compare("unkn")))
        {

            temp_value_ok=pow((atoi((*it1).at(3).c_str())-mean_value_ok),2);
            temp_quantity_ok=pow((atoi((*it1).at(2).c_str())-mean_quantity_ok),2);
            temp_value_ok=(const1_value_ok)*pow((const2_value_ok),temp_value_ok);
            temp_quantity_ok=(const1_quantity_ok)*pow((const2_quantity_ok),temp_quantity_ok);
            double ok=(double)((double)(number_ok)/(number_ok+number_fraud)*((values_ok[(*it1).at(0)]+(sample_size*prior_estimate_ok))/(number_ok+sample_size))*((values_ok[(*it1).at(1)]+(sample_size*prior_estimate_ok))/(number_ok+sample_size))*temp_quantity_ok*temp_value_ok);


            temp_value_fraud=pow((atoi((*it1).at(3).c_str())-mean_value_fraud),2);
            temp_quantity_fraud=pow((atoi((*it1).at(2).c_str())-mean_quantity_fraud),2);
            temp_value_fraud=(const1_value_fraud)*pow((const2_value_fraud),temp_value_fraud);
            temp_quantity_fraud=(const1_quantity_fraud)*pow((const2_quantity_fraud),temp_quantity_fraud);

            double fraud=(double)((double)(number_fraud)/(number_ok+number_fraud)*((values_fraud[(*it1).at(0)]+(sample_size*prior_estimate_fraud))/(number_fraud+sample_size))*((values_fraud[(*it1).at(1)]+(sample_size*prior_estimate_fraud))/(number_fraud+sample_size))*temp_quantity_fraud*temp_value_fraud);

            int i=0;
            while(i!=4)
            {
                /*    if(i==0 || i==1)
                    {
                            getoutdata<<"1"<<"\t";
                                getoutdata1<<"1"<<"\t";

                            }*/
                //  else
                //{
                getoutdata<<(*it1).at(i)<<"\t";
                getoutdata1<<(*it1).at(i)<<"\t";
                // }
                i++;
            }
            
            /* Once classified data, store all the details for that
            unknonw record in new file along with inferred label 
            using training data and Naive Bayes classification algorithm */
            
            //cout<<ok<<"   "<<fraud<<endl;
            if(ok<fraud)
            {
                test_fraud++;
                getoutdata1<<"fraud";
                getoutdata<<"fraud";
            }
            else
            {
                test_ok++;
                getoutdata1<<"ok";
                getoutdata<<"ok";
            }

            getoutdata<<endl;
            getoutdata1<<endl;
        }
    }
    cout<<endl;
    cout<<"Total time taken (In Seconds)   "<<((float)(clock()-total_time)/CLOCKS_PER_SEC)*2<<endl;
    cout<<"Number of unknown records classified as ok   "<<test_ok<<"  "<<endl;
    cout<<"Number of unknown records classified as fraud   "<<test_fraud<<"  "<<endl<<endl;
    cout<<endl<<"****************End of Naive Bayes****************"<<endl;
    getoutdata.close();
    getoutdata1.close();
    //getch();
    return 1;
}

