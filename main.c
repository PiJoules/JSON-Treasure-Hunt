#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int substringIsAtIndex(int substrLen, char substr[substrLen], int start, char* str);
char pop(char** stack, int* stackLen);
void add(char** stack, int* stackLen, char c);
void prependStr(char** stack, int* stackLen, int strLen, char str[strLen]);



/**
 * This program assumes nothing will go wrong: 
 * - the json is valid
 * - "dailyprogrammer" exists in the json somewhere.
 * - the first key always starts immediately after the
 *   opening curly bracket
 * - all strings contain alphanumeric characters
 *   - no quotes, commas, colons, etc.
 */
int main(int argc, char *argv[]){
	/* Read from stdin */
	int bufferSize = 256;
	char buffer[bufferSize];
	char* jsonString = (char*)malloc(1);
	int lineSize = 0;
	while (fgets(buffer, bufferSize, stdin) != NULL){
        if (*(buffer+strlen(buffer)-1) == '\n'){
            *(buffer+strlen(buffer)-1) = '\0';
        }
        int len = strlen(buffer);
        jsonString = (char*)realloc(jsonString, (lineSize+len)*sizeof(char));
        strncpy(jsonString + lineSize, buffer, len);
        lineSize += len;
	}
	int jsonStringLen = strlen(jsonString);



	int i;
	char dp[] = "dailyprogrammer";
	int dpLen = strlen(dp);
	int index = 0;
	char* stack = (char*)malloc(1); // Stack for keeping track of curly and square brackets
	int stackLen = 0;
	for (i = 0; i < jsonStringLen; i++){
		// Use a stack to keep track of keys and indeces
		char c = *(jsonString + i);
		if (c == '{' || c == '['){
			add(&stack, &stackLen, c);
		}
		else if (c == '}'){
			char c = pop(&stack, &stackLen);
			while (c != '{'){
				c = pop(&stack, &stackLen);
			}
		}
		else if (c == ']'){
			char c = pop(&stack, &stackLen);
			while (c != '['){
				c = pop(&stack, &stackLen);
			}
		}

		// Keep track of the substring
		int issubstr = substringIsAtIndex(dpLen, dp, i, jsonString);
		if (issubstr){
			int pathLen = 0;
			char* path = (char*)malloc(1);

			// Pop off the stack to keep track of keys and indeces
			int j = 2; // Start before the quote before dailyprogrammer
			char lastBracket = 0;
			while (stackLen > 0){
				char last = pop(&stack, &stackLen);

				if (last == '['){
					int commaCount = 0;
					int squareCount = 0; // Count the number of square brackets


					// Move the pointer out of the array
					if (lastBracket == '{'){
						int curlyCount = -1;
						while (curlyCount < 0){
							c = *(jsonString + i - j);

							if (c == '}'){
								curlyCount--;
							}
							else if (c == '{'){
								curlyCount++;
							}

							j++;
						}
					}
					while (squareCount < 1){
						c = *(jsonString + i - j);

						if (c == ']'){
							squareCount--;
						}
						else if (c == '['){
							squareCount++;
						}
						else if (c == ',' && squareCount == 0){
							commaCount++;
						}

						j++;
					}

					// Add the index to the path
					char numBuffer[bufferSize];
					memset(numBuffer, 0, bufferSize*sizeof(char));
					sprintf(numBuffer, "%d", commaCount);
					int numLen = strlen(numBuffer);
					prependStr(&path, &pathLen, 4, " -> ");
					prependStr(&path, &pathLen, numLen, numBuffer);

					lastBracket = '[';
				}
				else if (last == '{'){
					int startQuote = -1;
					int endQuote = -1;

					// Move the pointer out of the object
					if (lastBracket == '{'){
						int curlyCount = -1;
						while (curlyCount < 0){
							c = *(jsonString + i - j);

							if (c == '}'){
								curlyCount--;
							}
							else if (c == '{'){
								curlyCount++;
							}

							j++;
						}
					}
					while (startQuote < 0){
						c = *(jsonString + i - j);

						// Assume that the strings to the keys
						// are always surrounded by double quotes.
						if (c == '"' && endQuote < 0){
							endQuote = i-j;
						}
						else if (c == '"' && startQuote < 0){
							startQuote = i-j;
						}

						j++; // At this point, jsonString + i - j is before the startQuote
					}
					lastBracket = '{';

					// Add the string between these quotes to the path
					prependStr(&path, &pathLen, 4, " -> ");
					prependStr(&path, &pathLen, endQuote-startQuote-1, jsonString+i-j+2); // Start after the quote (j was also decremented in the last iteration)
				}
			}

			// Cheat and ignore the last 4 characters since they are " -> "
			*(path + pathLen-4) = 0;
			printf("%s\n", path);
			free(path);
			break;
		}
	}

	free(stack);
	free(jsonString);

	return 0;
}


/**
 * Check if the substring at start is the desired substring.
 * @param  substrLen Length of the substring
 * @param  substr    The substring
 * @param  start     Index in the original string to check
 * @param  str       The original string
 * @return           Boolean
 */
int substringIsAtIndex(int substrLen, char substr[substrLen], int start, char* str){
	int i;
	for (i = 0; i < substrLen; i++){
		char c = *(str + start + i);
		if (c != substr[i]){
			return 0;
		}
	}
	return 1;
}


/**
 * Pop the last character from a string.
 * Will change the size of the stack.
 * @param  stack    The string
 * @param  stackLen The string length
 * @return          The popped last character
 */
char pop(char** stack, int* stackLen){
	char c = *(*stack + *stackLen-1); // Get the last char
	*stackLen = *stackLen-1; // Adjust the length
	*stack = (char*)realloc(*stack, (*stackLen)*sizeof(char)); // Resize the stack
	return c;
}


/**
 * Add a character to the end of a string.
 * @param stack    The string
 * @param stackLen The length of the string
 * @param c 	   The length of the string
 */
void add(char** stack, int* stackLen, char c){
	*stackLen = *stackLen+1; // Adjust the length
	*stack = (char*)realloc(*stack, (*stackLen)*sizeof(char)); // Resize the stack
	*(*stack + *stackLen-1) = c; // Set the last char
}


/**
 * Prepend a string to a string.
 * @param stack    The string
 * @param stackLen The length of the string
 * @param strLen   The length of the string to prepend
 * @param str      The string to prepend
 */
void prependStr(char** stack, int* stackLen, int strLen, char str[strLen]){
	*stackLen = *stackLen+strLen; // Adjust the length
	*stack = (char*)realloc(*stack, (*stackLen)*sizeof(char)); // Resize the stack
	memmove(*stack+strLen, *stack, *stackLen-strLen);// Move the data
	strncpy(*stack, str, strLen);
}