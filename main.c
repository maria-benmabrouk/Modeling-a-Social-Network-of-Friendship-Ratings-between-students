#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#define MAX 100
#define b 20

//=====================================================================================================
//Define all required data structures of TOF
typedef struct{
    int fromStudentID;
    int toStudentID;
    int rating;
    int timestampOfTheRating;
}RecordTOF;


typedef struct{
    RecordTOF table[b];
    int NB;
}TypeBlockTOF,BufferTOF;

typedef struct{
    int NumberOfBlocks;
    int NumberOfRecords;
}headerTOF;

//Define all required data structures of LnOF
typedef struct{
    int fromStudentID;
    int toStudentID;
    int rating;
    int timestampOfTheRating;
    int deleted; // 0 if the record exist / 1:if the record is deleted
}RecordLnOF;

typedef struct TypeBlockLnOF{
    RecordLnOF table[b];
    int NB;
    int Next;
}TypeBlockLnOF,BufferLnOF;

typedef struct{
    int NumberOfFirstBlock;//the number of the block
    int NumberOfRecords;
    int NumberOfFinalBlock;
}headerLnOF;

typedef struct{
    FILE *f;
    headerLnOF *Head;
}LnOF;

/* ************************************************************************************************************************************************* */
//TOF======================================================================================================
//Initialize the header
void Initialize_Header_TOF(FILE*f){
    headerTOF Header;
    Header.NumberOfBlocks=0;
    Header.NumberOfRecords=0;
    fseek(f,0,SEEK_SET);
    fwrite(&Header,sizeof(headerTOF),1,f);
}

//Initialize TOF file and the header
void InitializeTOF(FILE **f,const char fileName[],const char mode){
    //create new binary file to read and write
    if(mode=='N'){
        *f=fopen(fileName,"wb+");
        Initialize_Header_TOF(*f);
    }else{
      if(mode=='E'){
         *f=fopen(fileName,"rb+");
         if(*f==NULL){
              *f=fopen(fileName,"wb+");
              if(*f==NULL){
                  perror("Error opening file\n");
                }
            }
        }else{
           printf("the mode is error\n");
             }
    }
}

// read a block from the TOF file and put it in the buffer
bool ReadBlockTOF(FILE *f,int blockNumber,BufferTOF *buffer){
    if(f == NULL || blockNumber<=0 || fseek(f,(blockNumber-1)*sizeof(TypeBlockTOF)+sizeof(headerTOF),SEEK_SET)!=0){
        return false;
    }
    if(fread(buffer,sizeof(BufferTOF),1,f)!=1){
        return false;
    }
    return true;
}

// write a block in the TOF file
bool WriteBlockTOF(FILE *f,int blockNumber,BufferTOF *buffer){
    if(f == NULL || blockNumber<=0 || fseek(f,(blockNumber-1)*sizeof(TypeBlockTOF)+sizeof(headerTOF),SEEK_SET)!=0){
        return false;
    }
    if(fwrite(buffer,sizeof(BufferTOF),1,f)!=1){
        return false;
    }
    return true;
}

//LnOF=====================================================================================================
//open LnOF file
void InitializeLnOF(LnOF *fileLnOF,const char fileName[],const char mode){
    fileLnOF->Head=malloc(sizeof(headerLnOF));
    if(mode=='N'){
        fileLnOF->f=fopen(fileName,"wb+");
        fileLnOF->Head->NumberOfFirstBlock=-1;
        fileLnOF->Head->NumberOfRecords=0;
        fileLnOF->Head->NumberOfFinalBlock=-1;
        fwrite(fileLnOF->Head,sizeof(headerLnOF),1,fileLnOF->f);
    }else{
       if(mode=='E'){
          fileLnOF->f=fopen(fileName,"rb+");
          if(fileLnOF->f==NULL){
            printf("Error opening LnOF file\n");
          }else{
              fread(fileLnOF->Head,sizeof(headerLnOF),1,fileLnOF->f);
          }
       }else{
       printf("the mode is error\n");
       }
    }
}

// Function to read a block from the file LnOF
void ReadBlockLnOF(LnOF *fileLnOF,int blockNumber,BufferLnOF *buffer){
    fseek(fileLnOF->f,(blockNumber-1)*sizeof(BufferLnOF)+sizeof(headerLnOF),SEEK_SET);
    fread(buffer,sizeof(BufferLnOF),1,fileLnOF->f);
}

// Function to write a block to the file LnOF
void WriteBlockLnOF(LnOF *fileLnOF,int blockNumber,BufferLnOF *buffer){
    fseek(fileLnOF->f,(blockNumber-1)*sizeof(BufferLnOF)+sizeof(headerLnOF),SEEK_SET);
    fwrite(buffer,sizeof(BufferLnOF),1,fileLnOF->f);
}

//=========================================================================================================
//close LnOF file
void CloseLnOF(LnOF*fileLnOF){
    rewind(fileLnOF->f);
    fwrite(fileLnOF->Head,sizeof(headerLnOF),1,fileLnOF->f);
    fclose(fileLnOF->f);
    free(fileLnOF->Head);
}

// edit the header of the file LnOF
void EditHeaderLnOF(LnOF *fileLnOF,int choice,int value){
    switch(choice){
        case 1:
            fileLnOF->Head->NumberOfFirstBlock=value;
            return;
        case 2:
            fileLnOF->Head->NumberOfRecords=value;
            return;
        case 3:
            fileLnOF->Head->NumberOfFinalBlock=value;
            return;
        default:
            printf("the number is not correct\n");
            return;
    }
}

// return selected field of the head of LnOF
void SelectedFieldHeadLnOF(LnOF *fileLnOF,int choice,int *value){
    switch(choice){
        case 1:
            *value=fileLnOF->Head->NumberOfFirstBlock;
            return;
        case 2:
            *value=fileLnOF->Head->NumberOfRecords;
            return;
        case 3:
            *value=fileLnOF->Head->NumberOfFinalBlock;
            return;
        default:
            printf("the number is not correct\n");
            return;
    }
}

// edit the header of the file TOF
void EditHeaderTOF(FILE *f,int choice,int value){
    headerTOF head;
    fseek(f,0,SEEK_SET);
    fread(&head,sizeof(headerTOF),1,f);
    switch(choice){
        case 1:
            head.NumberOfBlocks=value;
            break;
        case 2:
            head.NumberOfRecords=value;
            break;
        default:
            printf("the number is not correct\n");
            return;
    }
    fseek(f,0,SEEK_SET);
    fwrite(&head,sizeof(headerTOF),1,f);
}

// return selected field of the head of TOF
void ReturnFieldHeaderTOF(FILE *f,int choice,int *value){
    headerTOF head;
    fseek(f,0,SEEK_SET);
    fread(&head,sizeof(headerTOF),1,f);
    switch(choice){
        case 1:
            *value=head.NumberOfBlocks;
            return;
        case 2:
            *value=head.NumberOfRecords;
            return;
        default:
            printf("the number is not correct\n");
            return;
    }
}

// Count the number of the middle records in the dataset
int getMiddleRecordNumber(char fileName[]){
    FILE *f;
    f=fopen(fileName,"r");
    int count=0;
    char buf[MAX];
    while(fgets(buf,MAX,f)){
        count++;
    }
    fclose(f);
    return count/2;
}

// Count total records in dataset
int getTotalRecordNumber(char fileName[]){
    FILE *f;
    f=fopen(fileName,"r");
    int count=0;
    char buf[MAX];
    while(fgets(buf,MAX,f)){
        count++;
    }
    fclose(f);
    return count;
}
//*******************************************************************************************************************************************
//*******************************************************************************************************************************************
// comparison function for sorting records
// sort by ascending order according to first student id
int compareRecords(const void *a, const void *m){
    RecordTOF *r1 = (RecordTOF *)a;
    RecordTOF *r2 = (RecordTOF *)m;

    if(r1->fromStudentID != r2->fromStudentID){
        return r1->fromStudentID - r2->fromStudentID;
    }
    if(r1->toStudentID != r2->toStudentID){
        return r1->toStudentID - r2->toStudentID;
    }
    return r1->timestampOfTheRating - r2->timestampOfTheRating;
}

// Load data into TOF with 75% loading factor
void load_inTOF_sorted(FILE *tofFile, const char *txtfileName) {
    FILE *txtfile = fopen(txtfileName, "r");
    if (txtfile == NULL) {
        perror("failure to open text file\n");
        return;
    }

    int middleRecord = getMiddleRecordNumber(txtfileName);

    // allocate array to hold all records
    RecordTOF *records = (RecordTOF *)malloc(middleRecord * sizeof(RecordTOF));
    if(records == NULL){
        printf("Memory allocation failed!\n");
        fclose(txtfile);
        return;
    }

    // Read all records into array
    int count = 0;
    while (count < middleRecord && fscanf(txtfile, "%d %d %d %d",
            &records[count].fromStudentID,
            &records[count].toStudentID,
            &records[count].rating,
            &records[count].timestampOfTheRating) == 4) {
        count++;
    }
    fclose(txtfile);

    // Sort the records
    qsort(records, count, sizeof(RecordTOF), compareRecords);

    // Write sorted records to TOF with 75% loading factor
    int maxRecordsPerBlock = (int)(b * 0.75);
    int curBlck = 1;
    BufferTOF buffer;
    buffer.NB = 0;

    for(int i = 0; i < count; i++){
        if(buffer.NB >= maxRecordsPerBlock){
            WriteBlockTOF(tofFile, curBlck, &buffer);
            curBlck++;
            buffer.NB = 0;
        }
        buffer.table[buffer.NB] = records[i];
        buffer.NB++;
    }

    // Write last block if not empty
    if(buffer.NB > 0){
        WriteBlockTOF(tofFile, curBlck, &buffer);
    }

    // Update header
    EditHeaderTOF(tofFile, 1, curBlck);
    EditHeaderTOF(tofFile, 2, count);

    free(records);
    printf("Loaded %d records into %d blocks (sorted)\n", count, curBlck);
}
//*****************************************************************************************************************************************************
/* *****************************************************************TASK 2:************************************************************************* */
//*****************************************************************************************************************************************************
/*Insert N friendship ratings (from data file) while handling overflow using the Overflow File (LnOF)*/

// Find the appropriate block for a record in TOF
int findBlockForRecord(FILE *tofFile, RecordTOF record, int numBlocks){
    BufferTOF buffer;

    for(int i = 1; i <= numBlocks; i++){
        if(!ReadBlockTOF(tofFile, i, &buffer)){
            continue;
        }

        // Check if record belongs in this block
        if(buffer.NB > 0){
            RecordTOF firstRec = buffer.table[0];
            RecordTOF lastRec = buffer.table[buffer.NB - 1];

            // Record should go in this block if it's between first and last
            if(compareRecords(&record, &firstRec) >= 0 &&
               compareRecords(&record, &lastRec) <= 0){
                return i;
            }

            // If record is less than first record of this block, it should go in previous block
            if(compareRecords(&record, &firstRec) < 0){
                return (i > 1) ? i - 1 : i;
            }
        }
    }

    return numBlocks; // Goes in last block
}

// Insert record into overflow file (LnOF)
void insertIntoOverflow(LnOF *overflowFile, RecordTOF record, int blockNum){
    RecordLnOF lnofRecord;
    lnofRecord.fromStudentID = record.fromStudentID;
    lnofRecord.toStudentID = record.toStudentID;
    lnofRecord.rating = record.rating;
    lnofRecord.timestampOfTheRating = record.timestampOfTheRating;
    lnofRecord.deleted = 0;

    BufferLnOF buffer;
    int currentBlock, numRecords;

    SelectedFieldHeadLnOF(overflowFile, 1, &currentBlock);
    SelectedFieldHeadLnOF(overflowFile, 2, &numRecords);

    if(currentBlock == -1){
        // First overflow block
        buffer.NB = 0;
        buffer.Next = -1;
        buffer.table[buffer.NB++] = lnofRecord;
        WriteBlockLnOF(overflowFile, 1, &buffer);
        EditHeaderLnOF(overflowFile, 1, 1);
        EditHeaderLnOF(overflowFile, 3, 1);
        EditHeaderLnOF(overflowFile, 2, numRecords + 1);
    }else{
        // Find last block or first block with space
        int lastBlock;
        SelectedFieldHeadLnOF(overflowFile, 3, &lastBlock);
        ReadBlockLnOF(overflowFile, lastBlock, &buffer);

        if(buffer.NB < b){
            // Space available in last block
            buffer.table[buffer.NB++] = lnofRecord;
            WriteBlockLnOF(overflowFile, lastBlock, &buffer);
            EditHeaderLnOF(overflowFile, 2, numRecords + 1);
        }else{
            // Need new block
            BufferLnOF newBuffer;
            newBuffer.NB = 0;
            newBuffer.Next = -1;
            newBuffer.table[newBuffer.NB++] = lnofRecord;

            int newBlockNum = lastBlock + 1;
            WriteBlockLnOF(overflowFile, newBlockNum, &newBuffer);

            buffer.Next = newBlockNum;
            WriteBlockLnOF(overflowFile, lastBlock, &buffer);

            EditHeaderLnOF(overflowFile, 3, newBlockNum);
            EditHeaderLnOF(overflowFile, 2, numRecords + 1);
        }
    }
}
//find the position of the record in buffer
void findRecordPosition(BufferTOF buffer,RecordTOF record,int *position){
     int middle,last=buffer.NB-1;
     *position=0;
     while(*position<=last){
        middle=(*position+last)/2;
        if(compareRecords(&record,&buffer.table[middle])>0){
            *position=middle+1;
        }else{
            last=middle-1;
        }
     }
}
// insert record in buffer TOF
void InsertRecordInBufferTOF(BufferTOF *buffer,RecordTOF record){
    // buffer->table[buffer->NB++] = record;
    int position;
    findRecordPosition(*buffer,record,&position);
    for(int i=buffer->NB-1;i>=position;i--){
        buffer->table[i+1]=buffer->table[i];
    }

    buffer->table[position]=record;
    buffer->NB++;
}
// Insert N records from remaining 50% of data
void insertNRecords(FILE *tofFile, LnOF *overflowFile, const char *txtfileName, int N){
    FILE *txtfile = fopen(txtfileName, "r");
    if (txtfile == NULL) {
        perror("failure to open text file");
        return;
    }

    int middleRecord = getMiddleRecordNumber(txtfileName);
    int totalRecords = getTotalRecordNumber(txtfileName);
    int remainingRecords = totalRecords - middleRecord;

    if(N > remainingRecords){
        printf("Warning: Only %d records remaining. Inserting all.\n", remainingRecords);
        N = remainingRecords;
    }

    char buf[MAX];
    for(int i = 0; i < middleRecord; i++){
        fgets(buf, MAX, txtfile);
    }

    // Read and insert N records (from remaining 50%)
    RecordTOF record;
    int inserted = 0;
    int numBlocks, numRecords;

    while(inserted < N && fscanf(txtfile, "%d %d %d %d",
            &record.fromStudentID,
            &record.toStudentID,
            &record.rating,
            &record.timestampOfTheRating) == 4){

        ReturnFieldHeaderTOF(tofFile, 1, &numBlocks);
        ReturnFieldHeaderTOF(tofFile, 2, &numRecords);

        // Find appropriate block
        int targetBlock = findBlockForRecord(tofFile, record, numBlocks);
        BufferTOF buffer;
        ReadBlockTOF(tofFile, targetBlock, &buffer);

        int maxRecordsPerBlock = (int)(b );

        if(buffer.NB < maxRecordsPerBlock){
            // Space available in TOF block
           // buffer.table[buffer.NB++] = record;
            InsertRecordInBufferTOF(&buffer,record);
            WriteBlockTOF(tofFile, targetBlock, &buffer);
            EditHeaderTOF(tofFile, 2, numRecords + 1);
            printf("Inserted record (%d, %d) into TOF block %d\n",
                   record.fromStudentID, record.toStudentID, targetBlock);
        }else{
            // Block is full, insert into overflow
            insertIntoOverflow(overflowFile, record, targetBlock);
            printf("Inserted record (%d, %d) into overflow file\n",
                   record.fromStudentID, record.toStudentID);
        }

        inserted++;
    }

    fclose(txtfile);
    printf("\nTotal records inserted: %d\n", inserted);
}
void reorganizeFiles(FILE *TOFfile, LnOF *LnOFfile){
    int numberOfBlocksTOF, numberOfRecordsTOF, numberOfRecordsLnOF;
    int currentBlock, totalRecords = 0, recordIndex = 0;
    BufferTOF bufferTOF;
    BufferLnOF bufferLnOF;
    ReturnFieldHeaderTOF(TOFfile, 1, &numberOfBlocksTOF);
    ReturnFieldHeaderTOF(TOFfile, 2, &numberOfRecordsTOF);
    SelectedFieldHeadLnOF(LnOFfile, 2, &numberOfRecordsLnOF);
    totalRecords = numberOfRecordsTOF + numberOfRecordsLnOF;
    if(numberOfRecordsLnOF == 0){
        printf("No records in overflow file. Reorganization not needed.\n");
        return;
    }
    printf("Starting reorganization...\n");
    printf("TOF Records: %d, LnOF Records: %d, Total: %d\n",
           numberOfRecordsTOF, numberOfRecordsLnOF, totalRecords);
    // Allocate array to hold all records
    RecordTOF *allRecords = (RecordTOF *)malloc(totalRecords * sizeof(RecordTOF));
    if(allRecords == NULL){
        printf("the file is empty\n");
        return;
    }

    // Read all records from TOF
    for(int i = 1; i <= numberOfBlocksTOF; i++){
        if(ReadBlockTOF(TOFfile, i, &bufferTOF)){
            for(int j = 0; j < bufferTOF.NB; j++){
                allRecords[recordIndex++] = bufferTOF.table[j];
            }
        }
    }

    // Read all records from LnOF
    SelectedFieldHeadLnOF(LnOFfile, 1, &currentBlock);
    while(currentBlock != -1){
        ReadBlockLnOF(LnOFfile, currentBlock, &bufferLnOF);
        for(int i = 0; i < bufferLnOF.NB; i++){
            if(!bufferLnOF.table[i].deleted){
                allRecords[recordIndex].fromStudentID = bufferLnOF.table[i].fromStudentID;
                allRecords[recordIndex].toStudentID = bufferLnOF.table[i].toStudentID;
                allRecords[recordIndex].rating = bufferLnOF.table[i].rating;
                allRecords[recordIndex].timestampOfTheRating = bufferLnOF.table[i].timestampOfTheRating;
                recordIndex++;
            }
        }
        currentBlock = bufferLnOF.Next;
    }

    // Sort all records
    qsort(allRecords, recordIndex, sizeof(RecordTOF), compareRecords);

    // Rewrite sorted records back to TOF with 75% loading factor
    int maxRecordsPerBlock = (int)(b * 0.75);
    int curBlck = 1;
    BufferTOF newBuffer;
    newBuffer.NB = 0;

    for(int i = 0; i < recordIndex; i++){
        if(newBuffer.NB >= maxRecordsPerBlock){
            WriteBlockTOF(TOFfile, curBlck, &newBuffer);
            curBlck++;
            newBuffer.NB = 0;
        }
        newBuffer.table[newBuffer.NB] = allRecords[i];
        newBuffer.NB++;
    }

    if(newBuffer.NB > 0){
        WriteBlockTOF(TOFfile, curBlck, &newBuffer);
    }
    // Update TOF header
    EditHeaderTOF(TOFfile, 1, curBlck);
    EditHeaderTOF(TOFfile, 2, recordIndex);
    // Reset overflow file
    CloseLnOF(LnOFfile);
    InitializeLnOF(LnOFfile, "LnOF_file.bin", 'N');
    free(allRecords);
    printf("the file has been reorganized\n");
    printf("New TOF: %d records in %d blocks\n", recordIndex, curBlck);
}
/* ************************************display********************************** */
// Display TOF file content
void displayTOF(FILE *tofFile){
    int numBlocks, numRecords;
    ReturnFieldHeaderTOF(tofFile, 1, &numBlocks);
    ReturnFieldHeaderTOF(tofFile, 2, &numRecords);

    printf("\n========== TOF FILE CONTENT ==========\n");
    printf("Number of Blocks: %d\n", numBlocks);
    printf("Number of Records: %d\n", numRecords);
    printf("======================================\n\n");

    BufferTOF buffer;
    for(int i = 1; i <= numBlocks; i++){
        if(ReadBlockTOF(tofFile, i, &buffer)){
            printf("Block %d (Address: %ld bytes, Records: %d):\n",
                   i, (i-1)*sizeof(TypeBlockTOF)+sizeof(headerTOF), buffer.NB);
            for(int j = 0; j < buffer.NB; j++){
                printf("  [%d] From:%d To:%d Rating:%d Time:%d\n",
                       j+1,
                       buffer.table[j].fromStudentID,
                       buffer.table[j].toStudentID,
                       buffer.table[j].rating,
                       buffer.table[j].timestampOfTheRating);
            }
            printf("\n");
        }
    }
}

// Display specific block from TOF
void displayTOFBlock(FILE *tofFile, int blockNum){
    int numBlocks;
    ReturnFieldHeaderTOF(tofFile, 1, &numBlocks);

    if(blockNum < 1 || blockNum > numBlocks){
        printf("Invalid block number! Valid range: 1-%d\n", numBlocks);
        return;
    }

    BufferTOF buffer;
    if(ReadBlockTOF(tofFile, blockNum, &buffer)){
        printf("\n========== BLOCK %d ==========\n", blockNum);
        printf("Address: %ld bytes\n", (blockNum-1)*sizeof(TypeBlockTOF)+sizeof(headerTOF));
        printf("Number of Records: %d\n", buffer.NB);
        printf("======================================\n");
        for(int j = 0; j < buffer.NB; j++){
            printf("[%d] From:%d To:%d Rating:%d Time:%d\n",
                   j+1,
                   buffer.table[j].fromStudentID,
                   buffer.table[j].toStudentID,
                   buffer.table[j].rating,
                   buffer.table[j].timestampOfTheRating);
        }
        printf("\n");
    }
}

// Display overflow file content
void displayOverflow(LnOF *overflowFile){
    int firstBlock, numRecords, finalBlock;
    SelectedFieldHeadLnOF(overflowFile, 1, &firstBlock);
    SelectedFieldHeadLnOF(overflowFile, 2, &numRecords);
    SelectedFieldHeadLnOF(overflowFile, 3, &finalBlock);

    printf("\n========== OVERFLOW FILE (LnOF) ==========\n");
    printf("First Block: %d\n", firstBlock);
    printf("Final Block: %d\n", finalBlock);
    printf("Number of Records: %d\n", numRecords);
    printf("==========================================\n\n");

    if(firstBlock == -1){
        printf("Overflow file is empty.\n\n");
        return;
    }

    BufferLnOF buffer;
    int currentBlock = firstBlock;
    int blockCount = 1;

    while(currentBlock != -1){
        ReadBlockLnOF(overflowFile, currentBlock, &buffer);
        printf("Overflow Block %d (Records: %d, Next: %d):\n",
               currentBlock, buffer.NB, buffer.Next);
        for(int j = 0; j < buffer.NB; j++){
            if(!buffer.table[j].deleted){
                printf("  [%d] From:%d To:%d Rating:%d Time:%d\n",
                       j+1,
                       buffer.table[j].fromStudentID,
                       buffer.table[j].toStudentID,
                       buffer.table[j].rating,
                       buffer.table[j].timestampOfTheRating);
            }
        }
        printf("\n");
        currentBlock = buffer.Next;
        blockCount++;

        if(blockCount > 100){ // Safety check
            printf("Error: Possible infinite loop detected!\n");
            break;
        }
    }
}

// Menu system
void displayMenu(){
    printf("\n----------------------------------------------\n");
    printf("----SOCIAL NETWORK FILE MANAGEMENT SYSTEM ----\n");
    printf("----------------------------------------------\n");
    printf(" 1. Initialize and Load TOF (50%% sorted)   \n");
    printf(" 2. Insert N records (remaining 50%%)       \n");
    printf(" 3. Display entire TOF file                \n");
    printf(" 4. Display specific TOF block             \n");
    printf(" 5. Display overflow file (LnOF)           \n");
    printf(" 6. Display TOF header info                \n");
    printf(" 7. Search for the rating between two students\n");
    printf(" 8. Update an existing rating\n");
    printf(" 9. List all friends of a given student\n");
     printf("10. Reorganize the files \n");//merge TOF and LnOF
    printf(" 0. Exit                                    \n");
    printf("-----------------------------------------------\n");
    printf("Choose an option: ");
}
//*********************************************************************************************************************************
// Function to read a record from the TOF file
void readRecordTOF(FILE *TOFfile,int BlockNumber,int RecordNumber,RecordTOF *result){
    BufferTOF buffer;
    if(!ReadBlockTOF(TOFfile,BlockNumber,&buffer)){
        printf("Error: Failed to read block %d\n", BlockNumber);
        return;
    }
    if(RecordNumber < 0 || RecordNumber >= buffer.NB){
        printf("Error: Invalid record index %d in block %d\n", RecordNumber, BlockNumber);
        return;
    }
    *result=buffer.table[RecordNumber];
}
// Function to read a record from the LnOF file
void readRecordLnOF(LnOF *LnOFfile,int BlockNumber,int RecordNumber,RecordLnOF *result){
    BufferLnOF buffer;
    ReadBlockLnOF(LnOFfile,BlockNumber,&buffer);
    *result=buffer.table[RecordNumber];
}
//Search in TOF
typedef struct{
    int blockNumber;
    int recordNumber;
}recordPlace;
bool searchInTOF(FILE *TOFfile,int id1,int id2,recordPlace records[MAX],int *NumberOfRating){
    BufferTOF buffer;
    int numberOfBlocks;
    RecordTOF tofRecord;
    int indexRecords=0;
    *NumberOfRating=0;
    ReturnFieldHeaderTOF(TOFfile,1,&numberOfBlocks);
    for(int i=1;i<=numberOfBlocks;i++){
         if(ReadBlockTOF(TOFfile,i,&buffer)){
         for(int j=0;j<buffer.NB;j++){
                 tofRecord=buffer.table[j];
                 if(tofRecord.fromStudentID==id1){
                    if(tofRecord.toStudentID==id2){
                        records[indexRecords].blockNumber=i;
                        records[indexRecords].recordNumber=j;
                        indexRecords++;
                        (*NumberOfRating)++;
                    }
                 }
                 if(tofRecord.fromStudentID>id1 || (tofRecord.fromStudentID==id1 && tofRecord.toStudentID>id2)){
                    if(indexRecords==0){
                         return false;
                     }
                    return true;
                 }
         }
      }
    }
    if(indexRecords==0){
        return false;
    }
    return true;
}
//Search in LnOF
bool searchInLnOF(LnOF *LnOFfile,int id1,int id2,recordPlace records[MAX],int *NumberOfRating){
     BufferLnOF buffer;
     int currentBlock;
     int indexrecords=0;
     *NumberOfRating=0;
     if(LnOFfile==NULL || LnOFfile->f == NULL ){
        return false;
     }
     currentBlock=LnOFfile->Head->NumberOfFirstBlock;
     while(currentBlock!=-1){

        ReadBlockLnOF(LnOFfile,currentBlock,&buffer);
        for(int i=0;i<buffer.NB;i++){

            if(buffer.table[i].fromStudentID==id1 && buffer.table[i].toStudentID==id2){
                records[indexrecords].recordNumber=i;
                records[indexrecords].blockNumber=currentBlock;
                indexrecords++;
                (*NumberOfRating)++;

            }
        }
        currentBlock=buffer.Next;
     }
     if(indexrecords==0){

        return false;
     }

     return true;
}
//Search for the rating between two students.
void searchRatingBetTwoStudents(FILE *TOFfile,LnOF *LnOFfile,int Id1,int Id2){
    int NumberOfRatingTOF=0,NumberOfRatingLnOF=0,ratingNumber=0;
    RecordTOF Record_TOF;
    RecordLnOF Record_LnOF;
    recordPlace arrayTOF[MAX],arrayLnOF[MAX];
    bool a1,a2;
    a1=searchInTOF(TOFfile,Id1,Id2,arrayTOF,&NumberOfRatingTOF);
    a2=searchInLnOF(LnOFfile,Id1,Id2,arrayLnOF,&NumberOfRatingLnOF);
    if(!(a1 ||a2 )){
           printf("No rating exists between student %d and student %d\n", Id1, Id2);
           return;
       }
       printf("Found %d ratings between student %d and student %d:\n", NumberOfRatingTOF+NumberOfRatingLnOF, Id1, Id2);
    if(NumberOfRatingTOF!=0){
           for(int i=0;i<NumberOfRatingTOF;i++){
                  readRecordTOF(TOFfile,arrayTOF[i].blockNumber,arrayTOF[i].recordNumber,&Record_TOF);
                  ratingNumber++;
                  printf("    Rating %d : Student %d gave a rating of %d to student %d at time %d (in TOF)\n",ratingNumber, Id1, Record_TOF.rating , Id2, Record_TOF.timestampOfTheRating);
           }
    }
    if(NumberOfRatingLnOF!=0){
           for(int j=0;j<NumberOfRatingLnOF;j++){
                 readRecordLnOF(LnOFfile,arrayLnOF[j].blockNumber,arrayLnOF[j].recordNumber,&Record_LnOF);
                 ratingNumber++;
                  printf("    Rating %d : Student %d gave a rating of %d to student %d at time %d (in LnOF)\n", ratingNumber, Id1, Record_LnOF.rating , Id2, Record_LnOF.timestampOfTheRating);
           }
    }
}
void SearchRating(FILE *TOFfile,LnOF *LnOFfile){
    int Id1,Id2;
    printf("Enter the Id of the first student\n");
    scanf("%d",&Id1);
    printf("Enter the Id of the second student\n");
    scanf("%d",&Id2);
    searchRatingBetTwoStudents(TOFfile,LnOFfile,Id1,Id2);
    searchRatingBetTwoStudents(TOFfile,LnOFfile,Id2,Id1);
}
//**************************************************************************************************************************************
//Update an existing rating
void updateExistingRating(FILE *TOFfile,LnOF *LnOFfile){
     int id1,id2,newRating,timestamp,currentBlock,numberOfBlocks,exist=0;
     BufferLnOF buffer2;
     BufferTOF buffer1;
     printf("enter the from Id you want to change its rating\n");
     scanf("%d",&id1);
     printf("enter the to Id you want to change its rating\n");
     scanf("%d",&id2);
     printf("enter the timestamp\n");
     scanf("%d",&timestamp);
     printf("enter the new rating\n");
     scanf("%d",&newRating);
     if(newRating!=-1 || newRating!=1 || newRating!=2 || newRating!=3){
        printf("rating is not correct\n");
        return;
     }
     ReturnFieldHeaderTOF(TOFfile,1,&numberOfBlocks);
     for(int i=1;i<=numberOfBlocks;i++){
         if(ReadBlockTOF(TOFfile,i,&buffer1)){
         for(int j=0;j<buffer1.NB;j++){
                 if(buffer1.table[j].fromStudentID==id1 && buffer1.table[j].toStudentID==id2 && buffer1.table[j].timestampOfTheRating==timestamp){
                           exist=1;
                           buffer1.table[j].rating=newRating;
                           WriteBlockTOF(TOFfile,i,&buffer1);
                           printf("this rating is exist in the TOF file\n");
                           goto fin;
                }
            }
        }
     }
     if(LnOFfile==NULL || LnOFfile->f == NULL ){
         goto fin;
     }
     currentBlock=LnOFfile->Head->NumberOfFirstBlock;
     while(currentBlock!=-1){
        ReadBlockLnOF(LnOFfile,currentBlock,&buffer2);
        for(int i=0;i<buffer2.NB;i++){

            if(buffer2.table[i].fromStudentID==id1 && buffer2.table[i].toStudentID==id2 && buffer2.table[i].timestampOfTheRating==timestamp){
               exist=1;
               buffer2.table[i].rating=newRating;
               WriteBlockLnOF(LnOFfile,currentBlock,&buffer2);
               printf("this rating is exist in the overflow\n");
               goto fin;
            }
        }
        currentBlock=buffer2.Next;
     }
     fin:
    if(exist==0){
        printf("rating does not exist\n");
    }else{
        printf("Rating updated successfully\n");
    }
}
//**************************************************************************************************************************************
//Remove duplicate items in table
void removeDuplicate(int tab[],int *index){
    for(int i=0;i<(*index);i++){
        for(int j=i+1;j<(*index);j++){
            if(tab[i]==tab[j]){
                for(int k=j;k<(*index)-1;k++){
                    tab[k]=tab[k+1];
                }
                (*index)--;
                j--;
            }
        }
    }
}
//List all friends of a given student
void listAllFriendsOfStudent(FILE *TOFfile,LnOF *LnOFfile){
     int id,numberOfBlocks,indexTableFriends=0,indexTableBFF=0,currentBlock;
     BufferTOF buffer1;
     BufferLnOF buffer2;
     int tableFriends[MAX],tableBFF[MAX];
     printf("enter the id of the student you want to list all its friends\n");
     scanf("%d",&id);
     ReturnFieldHeaderTOF(TOFfile,1,&numberOfBlocks);
     for(int i=1;i<=numberOfBlocks;i++){
       if(ReadBlockTOF(TOFfile,i,&buffer1)){
         for(int j=0;j<buffer1.NB;j++){
                 if(buffer1.table[j].fromStudentID==id){
                    if(buffer1.table[j].rating==1 || buffer1.table[j].rating==2){
                        tableFriends[indexTableFriends]=buffer1.table[j].toStudentID;
                        indexTableFriends++;
                    }
                    if(buffer1.table[j].rating==3){
                        tableBFF[indexTableBFF]=buffer1.table[j].toStudentID;
                        indexTableBFF++;
                    }
                 }
                 if(buffer1.table[j].toStudentID==id){
                    if(buffer1.table[j].rating==1 || buffer1.table[j].rating==2){
                        tableFriends[indexTableFriends]=buffer1.table[j].fromStudentID;
                        indexTableFriends++;
                    }
                    if(buffer1.table[j].rating==3){
                        tableBFF[indexTableBFF]=buffer1.table[j].fromStudentID;
                        indexTableBFF++;
                    }
                 }
             }
         }
     }
     if(LnOFfile==NULL || LnOFfile->f == NULL ){
         goto fin;
     }
     currentBlock=LnOFfile->Head->NumberOfFirstBlock;
     while(currentBlock!=-1){
        ReadBlockLnOF(LnOFfile,currentBlock,&buffer2);
        for(int i=0;i<buffer2.NB;i++){
            if(buffer2.table[i].fromStudentID==id){
               if(buffer2.table[i].rating==1 || buffer2.table[i].rating==2 ){
                    tableFriends[indexTableFriends]=buffer2.table[i].toStudentID;
                    indexTableFriends++;
               }
               if(buffer2.table[i].rating==3){
                    tableBFF[indexTableBFF]=buffer2.table[i].toStudentID;
                    indexTableBFF++;
               }
            }
            if(buffer2.table[i].toStudentID==id){
               if(buffer2.table[i].rating==1 || buffer2.table[i].rating==2 ){
                    tableFriends[indexTableFriends]=buffer2.table[i].fromStudentID;
                    indexTableFriends++;
               }
               if(buffer2.table[i].rating==3){
                    tableBFF[indexTableBFF]=buffer2.table[i].fromStudentID;
                    indexTableBFF++;
               }
            }
        }
        currentBlock=buffer2.Next;
     }
     fin:
     if(indexTableFriends==0 && indexTableBFF==0){
        printf("this student has not friends\n");
     }else{
         if(indexTableFriends!=0){
           removeDuplicate(tableFriends,&indexTableFriends);
           printf("The friends of the student %d : \n",id);
           for(int i=0;i<indexTableFriends;i++){
             printf("%d, ",tableFriends[i]);
           }
           printf("\n");
        }
         if(indexTableBFF!=0){
            removeDuplicate(tableBFF,&indexTableBFF);
            printf("The best friends of the student %d : \n",id);
            for(int i=0;i<indexTableBFF;i++){
                printf("%d, ",tableBFF[i]);
            }
            printf("\n");
        }
     }
}
//**************************************************************************************************************************************
int main()
{
    FILE *tofFile = NULL;
    LnOF overflowFile;
    const char *tofFileName = "TOF_file.bin";
    const char *lnofFileName = "LnOF_file.bin";
    const char *txtFileName = "Dutch_college_social_network_randomized.txt";

    int choice;
    int tofInitialized = 0;
    int overflowInitialized = 0;

    do{
        displayMenu();
        scanf("%d", &choice);

        switch(choice){
            case 1:
                // Initialize and load TOF
                if(tofInitialized){
                    printf("TOF already initialized. Close and restart to reinitialize.\n");
                    break;
                }
                InitializeTOF(&tofFile, tofFileName, 'N');
                if(tofFile == NULL){
                    printf("Error opening TOF file.\n");
                    break;
                }
                load_inTOF_sorted(tofFile, txtFileName);
                tofInitialized = 1;

                // Initialize overflow file
                InitializeLnOF(&overflowFile, lnofFileName, 'N');
                overflowInitialized = 1;

                printf("\nTOF file loaded successfully!\n");
                break;

            case 2:
                // Insert N records
                if(!tofInitialized){
                    printf("Please initialize TOF first (option 1).\n");
                    break;
                }
                printf("Enter number of records to insert: ");
                int N;
                scanf("%d", &N);
                insertNRecords(tofFile, &overflowFile, txtFileName, N);
                break;

            case 3:
                // Display entire TOF
                if(!tofInitialized){
                    printf("Please initialize TOF first (option 1).\n");
                    break;
                }
                displayTOF(tofFile);
                break;

            case 4:
                // Display specific block
                if(!tofInitialized){
                    printf("Please initialize TOF first (option 1).\n");
                    break;
                }
                printf("Enter block number: ");
                int blockNum;
                scanf("%d", &blockNum);
                displayTOFBlock(tofFile, blockNum);
                break;

            case 5:
                // Display overflow file
                if(!overflowInitialized){
                    printf("Overflow file not yet created.\n");
                    break;
                }
                displayOverflow(&overflowFile);
                break;

            case 6:
                // Display header
                if(!tofInitialized){
                    printf("Please initialize TOF first (option 1).\n");
                    break;
                }
                int numBlocks, numRecords;
                ReturnFieldHeaderTOF(tofFile, 1, &numBlocks);
                ReturnFieldHeaderTOF(tofFile, 2, &numRecords);
                printf("\n========== TOF HEADER ==========\n");
                printf("Number of Blocks: %d\n", numBlocks);
                printf("Number of Records: %d\n", numRecords);
                printf("================================\n");
                break;
            case 7:
                if(!tofInitialized){
                    printf("Please initialize TOF first (option 1).\n");
                    break;
                }
                SearchRating(tofFile,&overflowFile);
                break;
            case 8:
                if(!tofInitialized){
                    printf("Please initialize TOF first (option 1).\n");
                    break;
                }
                updateExistingRating(tofFile,&overflowFile);
                break;
            case 9:
                if(!tofInitialized){
                    printf("Please initialize TOF first (option 1).\n");
                    break;
                }
                listAllFriendsOfStudent(tofFile,&overflowFile);
                break;
            case 10:
                 if(!tofInitialized){
                    printf("Please initialize TOF first (option 1).\n");
                    break;
                }
                reorganizeFiles(tofFile,&overflowFile);
                break;
            case 0:
                printf("Exiting program...\n");
                break;

            default:
                printf("Invalid option! Please try again.\n");
        }

    }while(choice != 0);

    // Cleanup
    if(tofInitialized && tofFile != NULL){
        fclose(tofFile);
    }
    if(overflowInitialized){
        CloseLnOF(&overflowFile);
    }

    return 0;
}
