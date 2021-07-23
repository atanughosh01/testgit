""" 9.4 Write a program to read through the mbox-short.txt and figure out who
has sent the greatest number of mail messages. The program looks for 'From '
lines and takes the second word of those lines as the person who sent the mail.
The program creates a Python dictionary that maps the sender's mail address to
a count of the number of times they appear in the file. After the dictionary is
produced, the program reads through the dictionary using a maximum loop to find
the most prolific committer. """

fname = input("Enter file name: ")                  # prompts user for file name
# guardian/conditional statement to avoid absurd filenames
if len(fname) < 1:                                  # if absurd filename taken as input
    fname = "mbox-short.txt"                        # considers filename as "mbox-short.txt"
else:                                               # else if proper filename taken as input
    try:                                            # tries to open the file
        fhand = open(fname)                         # opens the file chosen by user
    except FileNotFoundError:                       # catches exception if no such file is found
        print('File', fname, 'is not found')        # displays error message
        quit()                                      # quits the program

wordList = list()                                   # creating an empty list to store all the words later
for line in fhand:                                  # hovering through the lines in file
    if line.startswith('From '):                    # if line starts with 'From ' element
        line = line.split()                         # splits the line into a list of words
        wordList.append(line[1])                    # append the 2nd word of each line starting with 'From ' to a list

countDict = dict()                                  # creating an empty dictionary
for word in wordList:                               # hovering through word-s in list() of words
    countDict[word] = countDict.get(word, 0) + 1
# increment the count of an 'word' if it is already present in the dict()
# else if the 'word' absent in the dict(), add it to the dict()
# '0' is the default count if no such 'word' is present in the dict()

largest_count = None                                # initialising number of appearances of the word with most appearances to NULL
word_with_most_count = None                         # initialising the word with most number of appearances to NULL
count_list = countDict.items()                      # creates a list() of the words along with their counts as (key, value) pair
for word, count in count_list:                      # hovering through the list() of word-s along with their counts
    # conditional statement to check which word has the most number of appearances and how many times it appeared
    if largest_count is None or count > largest_count:
        word_with_most_count = word
        largest_count = count

# displaying the result
print('The mail id with most appearances :-', word_with_most_count)
print('Number of times that word appeared :-', largest_count)
