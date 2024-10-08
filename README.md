# regexrename
A simple utility to rename files following a naming pattern using regex capture.

### Usage

`regexrename <regex> <rename_pattern> <path_to_dir>`

In rename pattern, `$1` stands for first capture group, `$2` for second capture group and so on. `$0` will stand for the whole text that has been matched. You can use `\$` if you want to show a literal `$` sign and `\\` to show a backslash.

### Example
Imagine you have a set of files which are named using a pattern. For this example we will be using images named using date.

Let's take a few files here, we are assuming it's in a directory named images in our working directory:

12-06-2023.png
09-12-2022.png
25-05-2023.png

If we use normal alphabetic ordering for these files it would not show up in a latest-date-first manner. To solve this, we would have to change the ordering a bit and make it into YYYY-MM-DD styled ordering.

`regexrename` accepts 3 arguments by default, third one being the path to the directory which contains files to be renamed, second one being the regex itself to match and capture data, and first one the pattern to rearrange it to. In the second argument, $1 stands for the first capture group, $2 for the second, and so on. So to rename these files to the pattern we require, we would use:

`regexrename '(\d+)-(\d+)-(\d+)\.(.*)' '$3-$2-$1.$4' images/`

So this will rename our files to the format we need:

2023-06-12.png
2023-05-25.png
2022-12-09.png

Of course, this is only a simple usecase this utility has. The regex can be used to capture text from filenames in a variety of combinations, making it quite a powerful utility.

That's it, thanks!
