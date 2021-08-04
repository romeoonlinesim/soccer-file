#include	<stdio.h>
#include	<stdlib.h>
#include     <time.h>
#include     <string.h>
#include     <ctype.h>

int main(void)
{
	FILE	*fm1,*fm2,*fm3,*fmp, *t,*fk1,*fk2,*fk3,*fkp;
	char	s[256];
	double	tackle1,tackle2,checkball,drib,g;
	double z[48];
	int i,n ,x,a,y;
	char    name[256],cpp[256],h[256],no[10],team[256],kname[256],kcpp[256],kh[256];
	char *p;

y=0;
a=0;

/* パラメータファイルオープン読み込み */
t = fopen("randamparameter.txt", "r");
    if(t == NULL)
        exit(0);

    while( fscanf(t, "%lf", &g) != EOF ){
    	z[y]=g;
    	y=y+1;
    }
    fclose(t);


/*x=1の時ディフェンス、x=2の時オフェンスを作成*/
	
	for(x=1;x <= 2; x++)
	{
		strcpy(no,"0");
		p = no;
		
		for(n=1; n <= 6; n++)
		{

/* ファイル名をつける */

			if(x==1){
				strcpy(name, "defense");
				
			}
			else{
				strcpy(name,"offense");
				
			}
			
			strcpy(kname,name);

			*p = *p + 1;
			strcat(name,"_move");
			strcat(name,no );
			strcpy(cpp, name);
			strcpy(h, name);
			strcat(cpp, ".cpp");
			strcat(h, ".h");

/* nameを大文字に変換 */

			i = 0;
			while ( name[i] != '\0' ) {
			name[i] = toupper( name[i] );
			i++;
			}

/* kickのファイルに名前をつける*/

			strcat(kname,"_kick");
			strcat(kname,no );
			strcpy(kcpp, kname);
			strcpy(kh, kname);
			strcat(kcpp, ".cpp");
			strcat(kh, ".h");

/* knameを大文字に変換 */

			i = 0;
			while ( kname[i] != '\0' ) {
			kname[i] = toupper( kname[i] );
			i++;
			}
			
/* 入力ファイルオープン */
			
			if( (fm1=fopen("m1.cpp","r"))==NULL) {
			printf("M1入力ファイルがオープンできません\n");
			exit(EXIT_FAILURE);
			}
			if( (fm2=fopen("m2.cpp","r"))==NULL) {
			printf("M2入力ファイルがオープンできません\n");
			exit(EXIT_FAILURE);
			}
			if( (fm3=fopen("m3.cpp","r"))==NULL) {
			printf("M3入力ファイルがオープンできません\n");
			exit(EXIT_FAILURE);
			}

			if( (fk1=fopen("k1.cpp","r"))==NULL) {
			printf("K1入力ファイルがオープンできません\n");
			exit(EXIT_FAILURE);
			}
			if( (fk2=fopen("k2.cpp","r"))==NULL) {
			printf("K3入力ファイルがオープンできません\n");
			exit(EXIT_FAILURE);
			}
			if( (fk3=fopen("k3.cpp","r"))==NULL) {
			printf("K3入力ファイルがオープンできません\n");
			exit(EXIT_FAILURE);
			}

/* 出力ファイルオープン */

			if( (fmp=fopen(cpp,"w"))==NULL) {
			printf("動きの出力ファイルがオープンできません\n");
			exit(EXIT_FAILURE);
			}

			if( (fkp=fopen(kcpp,"w"))==NULL) {
			printf("キック出力ファイルがオープンできません\n");
			exit(EXIT_FAILURE);
			}
			
			tackle1=z[a];
			tackle2=z[a+1];
			checkball=z[a+2];
			drib=z[a+3];
			
			a=a+4;
			

 /*cppへ書き込み*/
			fprintf(fmp,"\n#ifdef HAVE_CONFIG_H\n#include <config.h>\n#endif\n\n#include \"%s\"\n",h);

/*入力ファイルm1.cppから読み込んだデータを出力ファイルmoveに書き込み*/

			while(fgets(s,256,fm1)!=NULL) {		
				fputs(s,fmp);	
			}

			fprintf(fmp,"%s::execute( PlayerAgent * agent )\n{\n",name);
			fprintf(fmp,"	dlog.addText( Logger::TEAM,\n");
			fprintf(fmp,"					__FILE__\": %s\" );\n\n // tackle\n\n",name);    
			fprintf(fmp,"if ( Bhv_BasicTackle( %g, %g ).execute( agent ) )",tackle1,tackle2);

/*入力ファイルm２.cppから読み込んだデータを出力ファイルmoveに書き込み*/

			while(fgets(s,256,fm2)!=NULL) {
				fputs(s,fmp);			
			}

			fprintf(fmp,"         && ( self_min <= %g \n",checkball);

/*入力ファイルm3.cppから読み込んだデータを出力ファイルmoveに書き込み*/

			while(fgets(s,256,fm3)!=NULL) {
				fputs(s,fmp);		
			}

//cppへの書き込み終了

/* kick.cppの書き込み	*/


			fprintf(fkp,"#ifdef HAVE_CONFIG_H\n#include <config.h>\n#endif\n\n#include \"%s\" \n",kh);

/*入力ファイルk1.cppから読み込んだデータを出力ファイルkickに書き込み*/

			while(fgets(s,256,fk1)!=NULL) {	
				fputs(s,fkp);			
			}

			fprintf(fkp,"%s::execute( PlayerAgent * agent )\n{\n",kname);
			fprintf(fkp,"	dlog.addText( Logger::TEAM,\n");
			fprintf(fkp,"						__FILE__\": %s\" );//変更を加える\n",kname);

			if(drib == 0){

/*ドリブルが０の時、入力ファイルk2.cppから読み込んだデータを出力ファイルkickに書き込み*/

				while(fgets(s,256,fk2)!=NULL) {	
					fputs(s,fkp);			
				}
			}

/*ドリブルが１の時、入力ファイルk3.cppから読み込んだデータを出力ファイルkickに書き込み*/

			else{
				while(fgets(s,256,fk3)!=NULL) {	
					fputs(s,fkp);			
				}
			}


			
/* 入力ファイルクローズ */
			fclose(fm1);
			fclose(fm2);	
			fclose(fm3);	

			fclose(fk1);
			fclose(fk2);
			
/* 出力ファイルクローズ */
			fclose(fmp);	
			fclose(fkp);

		}
	}
	return 0;
}
