#include <stdio.h>
#include <string.h>

int main()
{
	FILE* fp = fopen("fcf.txt", "rb");
	FILE* fout = fopen("out.txt", "wb+");
	if(!fp)
		return 0;

	char buff[512];
	int num_sections;
	float a[6];
	int nScales = 0;

	while(feof(fp) == 0)
	{
		fgets (buff, sizeof(buff), fp);
		if(sscanf(buff, "Number of Sections  : %d", &num_sections) == 1)
		{

			fprintf(fout, "float a[%d][3], b[%d][3];\n", num_sections, num_sections);
			fprintf(fout, "float gain_in[%d], gain_out;\n", num_sections);
			fprintf(fout, "float x, y;\n");
			fprintf(fout, "float w[%d][3];\n", num_sections);
			fprintf(fout, "int num_sections;\n");
			fprintf(fout, "\n");
		}

		else if(!strncmp(buff, "SOS matrix:", 11))
		{
			for(int i = 0; i < num_sections; ++i)
			{
				fgets (buff, sizeof(buff), fp);
				if(sscanf(buff, "%f %f %f %f %f %f", 
					&a[0], &a[1], &a[2], &a[3], &a[4], &a[5]) == 6)
				{
					fprintf(fout, "a[%d][0] = %.10ff;\n", i, a[3]);
					fprintf(fout, "a[%d][1] = %.10ff;\n", i, a[4]);
					fprintf(fout, "a[%d][2] = %.10ff;\n", i, a[5]);
					fprintf(fout, "b[%d][0] = %.10ff;\n", i, a[0]);
					fprintf(fout, "b[%d][1] = %.10ff;\n", i, a[1]);
					fprintf(fout, "b[%d][2] = %.10ff;\n", i, a[2]);
					fprintf(fout, "\n");
				}
			}
			
		}

		else if(!strncmp(buff, "Scale Values:", 12))
		{
			for(int i = 0; i < num_sections + 1; ++i)
			{
				while(!feof(fp))
				{
					fgets (buff, sizeof(buff), fp);
					if(sscanf(buff, "%f", &a[0]) == 1)
					{
						fprintf(fout, "gain_in[%d] = %.10ff;\n", i, a[0]);
						nScales++;
					}
				}
			}
		}
	}

	for(int i = nScales - 1; i < num_sections; ++i)
		fprintf(fout, "gain_in[%d] = 1.0f;\n", i);
	fprintf(fout, "gain_out = 1.0f;\n");
	
	fprintf(fout, "\nnum_sections = %d;\n", num_sections);
	fclose(fp);
	fclose(fout);
}