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

/* �p�����[�^�t�@�C���I�[�v���ǂݍ��� */
t = fopen("randamparameter.txt", "r");
    if(t == NULL)
        exit(0);

    while( fscanf(t, "%lf", &g) != EOF ){
    	z[y]=g;
    	y=y+1;
    }
    fclose(t);


/*x=1�̎��f�B�t�F���X�Ax=2�̎��I�t�F���X���쐬*/
	
	for(x=1;x <= 2; x++)
	{
		strcpy(no,"0");
		p = no;
		
		for(n=1; n <= 6; n++)
		{

/* �t�@�C���������� */

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

/* name��啶���ɕϊ� */

			i = 0;
			while ( name[i] != '\0' ) {
			name[i] = toupper( name[i] );
			i++;
			}

/* kick�̃t�@�C���ɖ��O������*/

			strcat(kname,"_kick");
			strcat(kname,no );
			strcpy(kcpp, kname);
			strcpy(kh, kname);
			strcat(kcpp, ".cpp");
			strcat(kh, ".h");

/* kname��啶���ɕϊ� */

			i = 0;
			while ( kname[i] != '\0' ) {
			kname[i] = toupper( kname[i] );
			i++;
			}
			
/* ���̓t�@�C���I�[�v�� */
			
			if( (fm1=fopen("m1.cpp","r"))==NULL) {
			printf("M1���̓t�@�C�����I�[�v���ł��܂���\n");
			exit(EXIT_FAILURE);
			}
			if( (fm2=fopen("m2.cpp","r"))==NULL) {
			printf("M2���̓t�@�C�����I�[�v���ł��܂���\n");
			exit(EXIT_FAILURE);
			}
			if( (fm3=fopen("m3.cpp","r"))==NULL) {
			printf("M3���̓t�@�C�����I�[�v���ł��܂���\n");
			exit(EXIT_FAILURE);
			}

			if( (fk1=fopen("k1.cpp","r"))==NULL) {
			printf("K1���̓t�@�C�����I�[�v���ł��܂���\n");
			exit(EXIT_FAILURE);
			}
			if( (fk2=fopen("k2.cpp","r"))==NULL) {
			printf("K3���̓t�@�C�����I�[�v���ł��܂���\n");
			exit(EXIT_FAILURE);
			}
			if( (fk3=fopen("k3.cpp","r"))==NULL) {
			printf("K3���̓t�@�C�����I�[�v���ł��܂���\n");
			exit(EXIT_FAILURE);
			}

/* �o�̓t�@�C���I�[�v�� */

			if( (fmp=fopen(cpp,"w"))==NULL) {
			printf("�����̏o�̓t�@�C�����I�[�v���ł��܂���\n");
			exit(EXIT_FAILURE);
			}

			if( (fkp=fopen(kcpp,"w"))==NULL) {
			printf("�L�b�N�o�̓t�@�C�����I�[�v���ł��܂���\n");
			exit(EXIT_FAILURE);
			}
			
			tackle1=z[a];
			tackle2=z[a+1];
			checkball=z[a+2];
			drib=z[a+3];
			
			a=a+4;
			

 /*cpp�֏�������*/
			fprintf(fmp,"\n#ifdef HAVE_CONFIG_H\n#include <config.h>\n#endif\n\n#include \"%s\"\n",h);

/*���̓t�@�C��m1.cpp����ǂݍ��񂾃f�[�^���o�̓t�@�C��move�ɏ�������*/

			while(fgets(s,256,fm1)!=NULL) {		
				fputs(s,fmp);	
			}

			fprintf(fmp,"%s::execute( PlayerAgent * agent )\n{\n",name);
			fprintf(fmp,"	dlog.addText( Logger::TEAM,\n");
			fprintf(fmp,"					__FILE__\": %s\" );\n\n // tackle\n\n",name);    
			fprintf(fmp,"if ( Bhv_BasicTackle( %g, %g ).execute( agent ) )",tackle1,tackle2);

/*���̓t�@�C��m�Q.cpp����ǂݍ��񂾃f�[�^���o�̓t�@�C��move�ɏ�������*/

			while(fgets(s,256,fm2)!=NULL) {
				fputs(s,fmp);			
			}

			fprintf(fmp,"         && ( self_min <= %g \n",checkball);

/*���̓t�@�C��m3.cpp����ǂݍ��񂾃f�[�^���o�̓t�@�C��move�ɏ�������*/

			while(fgets(s,256,fm3)!=NULL) {
				fputs(s,fmp);		
			}

//cpp�ւ̏������ݏI��

/* kick.cpp�̏�������	*/


			fprintf(fkp,"#ifdef HAVE_CONFIG_H\n#include <config.h>\n#endif\n\n#include \"%s\" \n",kh);

/*���̓t�@�C��k1.cpp����ǂݍ��񂾃f�[�^���o�̓t�@�C��kick�ɏ�������*/

			while(fgets(s,256,fk1)!=NULL) {	
				fputs(s,fkp);			
			}

			fprintf(fkp,"%s::execute( PlayerAgent * agent )\n{\n",kname);
			fprintf(fkp,"	dlog.addText( Logger::TEAM,\n");
			fprintf(fkp,"						__FILE__\": %s\" );//�ύX��������\n",kname);

			if(drib == 0){

/*�h���u�����O�̎��A���̓t�@�C��k2.cpp����ǂݍ��񂾃f�[�^���o�̓t�@�C��kick�ɏ�������*/

				while(fgets(s,256,fk2)!=NULL) {	
					fputs(s,fkp);			
				}
			}

/*�h���u�����P�̎��A���̓t�@�C��k3.cpp����ǂݍ��񂾃f�[�^���o�̓t�@�C��kick�ɏ�������*/

			else{
				while(fgets(s,256,fk3)!=NULL) {	
					fputs(s,fkp);			
				}
			}


			
/* ���̓t�@�C���N���[�Y */
			fclose(fm1);
			fclose(fm2);	
			fclose(fm3);	

			fclose(fk1);
			fclose(fk2);
			
/* �o�̓t�@�C���N���[�Y */
			fclose(fmp);	
			fclose(fkp);

		}
	}
	return 0;
}
