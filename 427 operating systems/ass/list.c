int compare(const char *str1, const char *str2) {
	// printf("comparing: *s: %d, s: %s\n", *str1, str1);
    if (*str1 == '\0' && *str2 == '\0') return 0;
    if (*str1 == '\0') return -1;
    if (*str2 == '\0') return 1;

    if (isalpha(*str1) && isalpha(*str2)) { //both are alphabets
        if (isupper(*str1) && islower(*str2)) { 
			// printf("1 upper: str1, str2: %c, %c\n", *str1, *str2);
			// printf("converted: str1, str2: %c, %c\n", *str1+'a'-'A', *str2);
			if(*str1+'a'-'A'==*str2) { //same letter, 1 upper
				return 1;
			}
			// printf("result of *str1+'a'-'A'>*str2: %d\n", *str1+'a'-'A'>*str2);
			if(*str1+'a'-'A'>*str2){
				return 1;
			} else return -1;
		}
        else if (isupper(*str2) && islower(*str1)) {
			if(*str2+'a'-'A'==*str1) { //same letter, 2 upper
				return -1;
			}
			if(*str1>'a'-'A'+*str2) {
				return 1;
			} else return -1;
		}
        else { //2 upper or 2 lower
			if (*str1 == *str2) { 
				return compare(str1+1, str2+1); //same letter, compare at next index
			} else {
				printf("result of *str1>*str2: %d\n",*str1>*str2);
				if(*str1>*str2){
					return 1;
				} else return -1;
			}
		}
    } else { //one or both is not letter
		if (*str1 == *str2) { //same character
			return compare(str1+1, str2+1); //compare at next index
		} else {
			if(*str1>*str2) {
				return 1; 
			} else return -1;
		}
	}
}
int list(){
	struct dirent *d;
	DIR *dh = opendir(".");
	char dirs[200][200];
	int count=0;
	char tmp[200];
	while((d=readdir(dh))!= NULL){
		strcpy(dirs[count],d->d_name);
		count++;
	}
	// int result = 0;
	for(int i =0; i<count; i++){
		int min =i;
		for(int j=i; j<count; j++){
			if(compare(dirs[j],dirs[min])<0){
				min = j;
			}
		}
		strcpy(tmp,dirs[min]);
		strcpy(dirs[min],dirs[i]);
		strcpy(dirs[i],tmp);
	}
	for(int i=0; i< count; i++){
		printf("%s\n", dirs[i]);
	}
	closedir(dh);
	return 0;
}