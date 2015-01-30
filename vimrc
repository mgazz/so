"------------NOTE
"look to fatih vimrc (really cool!)

"
"import vundle plugin
"
source	~/.vim/vundle-plugins.vim


"set mapleader space and not \ 
let mapleader = "\<Space>"
"fast save quit
nnoremap <Leader>w :w<CR>
nnoremap <Leader>q :q<CR>
"Enter visual line mode with <Space><Space>
nmap <Leader><Leader> V
"shortcut to rapidly toggle 'set list'
nmap <leader>l :set list!<CR>
"change number column from normal to relative
"nnoremap <leader>r :call NumberToggle()<CR>
" Quickly edit/reload the vimrc file
nmap <silent> <leader>vv :vs <CR> -
"nmap <silent> <leader>e :e 
nmap <silent> <leader>ev :e ~/.vimrc<CR>
nmap <silent> <leader>vu :vs ~/.vim/vundle-plugins.vim<CR>
nmap <silent> <leader>sv :so $MYVIMRC<CR>


"
"default setting
"
"testing
"
set shortmess=a
set cmdheight=2
""""
syntax on
"enable number in page
set number
"copy indent from currentline when starting a new line
set autoindent 
"number of spaces that a <Tab> in the file contrs for
set tabstop=2
"number of spaces that <Tab> conts for while performing editing operations
set softtabstop=2
"determine the amount of spaces to insert or remove using the indentation comands in normal mode
set shiftwidth=2
"hidden to navigate buffet without save
set hidden 
"cause spaces to be used in place of <Tab>
set noexpandtab
"We are running Vim not Vi
set nocompatible
" Open new split panes to right and bottom, which feels more natural
set splitbelow
set splitright
"set backspace
set backspace=2
"" Always show the status line
set laststatus=2
"show commands
set showcmd
"highlight search
set hlsearch
set incsearch


"
"formatting
"
"ruler setting
set rulerformat=%-14.(%l,%c%V%)\ %P
"use the same symbol as TextMate for tabstop and EOL
set listchars=tab:▸\ ,eol:¬

"
colorscheme molokai
hi MatchParen			 ctermfg=208	ctermbg=233 cterm=bold

"
"command insert mode
"
inoremap <C-l> <right>

"set tabstop, softtabstop and shiftwidth to the same value
command! -nargs=* Stab call Stab()
command! Home execute "e ~/"
command! CpMan :call CscopeProjectManagement()
command! CpCrate :call CscopeCreate()
command! CpUpdate	:call CscopeCreate()

nnoremap <leader>r :call Replace()<cr>





"
" window management
"
" Quicker window movement
nnoremap <C-j> <C-w>j 
nnoremap <C-k> <C-w>k
nnoremap <C-h> <C-w>h<CR>
nnoremap <C-l> <C-w>l<CR>
"nnoremap <silent> <Leader>rl :call VerticalShrink()<CR>
"nnoremap <silent> <Leader>rh :call VerticalEnlarge()<CR>
"Show variables and methods
map <F9> :TagbarToggle<CR>
nnoremap <silent> <Leader>tt :TagbarToggle<CR>

"
"registers management

"
"Buffer management
"
" cycle back/forward recently viewed buffers
nnoremap <C-n> :bn<cr>
"nnoremap <C-m> :bn<cr>
"nunmap <Leader>b
map <Leader>bo :BufExplorer<CR>
" Close the current buffer
map <leader>bc :Bclose<cr>
" Remember info about open buffers on close
set viminfo^=%

"
"disable mapping
"
"nunmap <CR>

"
"tab settings
"
"" Useful mappings for managing tabs
map <leader>tn :tabnew<cr>
map <leader>to :tabonly<cr>
map <leader>tc :tabclose<cr>
map <leader>tm :tabnext<cr>
" Opens a new tab with the current buffer's path
" Super useful when editing files in the same directory
map <leader>te :tabedit <c-r>=expand("%:p:h")<cr>/


"
"Vim autocmd settings
"
"disable highlight when enter insert mode
autocmd InsertEnter * :setlocal nohlsearch
autocmd InsertLeave * :setlocal hlsearch
" Return to last edit position when opening files (You want this!)
autocmd BufReadPost *
		 \ if line("'\"") > 0 && line("'\"") <= line("$") |
		 \	 exe "normal! g`\"" |
		 \ endif
"autoload cscope db
"autocmd VimEnter * :call CscopeProjectManagement()

"
"Java settings
"
au FileType java nmap <Leader>jc :JavaCorrect<cr>
"
"Ruby settings
"
au BufRead,BufNewFile *.rb set filetype=ruby

"
"Golang setting
"
autocmd FileType go compiler go
au BufRead,BufNewFile *.go set filetype=go
"Show a list of interfaces which is implemented by the type under your cursor
au FileType go nmap <Leader>gs <Plug>(go-implements)
"Show type info for the word under your cursor
au FileType go nmap <Leader>gi <Plug>(go-info)
"Open the relevant Godoc for the word under the cursor
au FileType go nmap <Leader>gd <Plug>(go-doc)
au FileType go nmap <Leader>gv <Plug>(go-doc-vertical)
"Open the Godoc in browser
au FileType go nmap <Leader>gb <Plug>(go-doc-browser)
"Run commands
au FileType go nmap <leader>gr <Plug>(go-run)
au FileType go nmap <leader>gb <Plug>(go-build)
au FileType go nmap <leader>gt <Plug>(go-test)
au FileType go nmap <leader>gc <Plug>(go-coverage)
"By default gd is enabled which opens the target identifier in current buffer. 
"You can also open in vertical, horizontal or tab for the word under your cursor:
au FileType go nmap <Leader>ds <Plug>(go-def-split)
au FileType go nmap <Leader>dv <Plug>(go-def-vertical)
au FileType go nmap <Leader>dt <Plug>(go-def-tab)
"Rename the identifier under the cursor to a new name
au FileType go nmap <Leader>e <Plug>(go-rename)

"Disable opening browser after posting to your snippet to play.golang.org:
let g:go_play_open_browser = 0
"By default vim-go shows errors for the fmt command, to disable it:
let g:go_fmt_fail_silently = 1
"Enable goimports to automatically insert import paths instead of gofmt:
let g:go_fmt_command = "goimports"
"Disable auto fmt on save:
let g:go_fmt_autosave = 0
"By default binaries are installed to $GOBIN or $GOPATH/bin. To change it:
"let g:go_bin_path = expand("~/.gotools")
"let g:go_bin_path ="/home/fatih/.mypath"			 
let g:go_highlight_functions = 1
let g:go_highlight_methods = 1
let g:go_highlight_structs = 1
let g:go_highlight_operators = 1
let g:go_highlight_extra_types = 1

"
"Python settings
au FileType python set tabstop=4|set shiftwidth=4|set softtabstop=4|set noexpandtab

"
"ctags cscope settings
"
nmap <Leader>Ct :!ctags -R --exclude=.git --exclude=log --exclude=tmp *<CR>
nmap <Leader>Cc :call CscopeCreate()<CR>
"go to definition and return
nnoremap <[ <C-]> 
nnoremap <] <C-t> 

"
"airline
"
" Enable the list of buffers
let g:airline#extensions#tabline#enabled = 1
" Show just the filename
let g:airline#extensions#tabline#fnamemod = ':t'
"
"Ultisnips settings
"
let g:UltiSnipsSnippetDirectories = ["UltiSnips"]
let g:UltiSnipsExpandTrigger = "<C-j>"
let g:UltiSnipsListSnippets = "<C-tab>"
let g:UltiSnipsJumpForwardTrigger = "<C-j>"
let g:UltiSnipsJumpBackwardTrigger = "<C-k>"


"
"YouCompleteMe settings
"
let g:ycm_autoclose_preview_window_after_completion = 1

"
"Eclim
"
let g:EclimCompletionMethod = 'omnifunc'

"------------------- to prove ----------------------- 

"" Close all the buffers
"map <leader>ba :1,1000 bd!<cr>


"" Switch CWD (current working directory) to the directory of the open buffer
"map <leader>cd :cd %:p:h<cr>:pwd<cr>

"" Specify the behavior when switching between buffers 
"try
	"set switchbuf=useopen,usetab,newtab
	"set stal=2
"catch
"endtry


function! Replace()
" corretto.. cambia solo colore della parola evideziata 	
	let l:old = expand("<cword>")
	let l:new= input('R word:')
	let l:lines= 1 * input('lines:')
	exe ":.,.+" . l:lines."s/" . l:old . "/" . l:new . "/cg"
endfunction

function! CmdLine(str)
		exe "menu Foo.Bar :" . a:str
		emenu Foo.Bar
		unmenu Foo
endfunction

function! CscopeProjectManagement()
	let l:current_path = getcwd() 
	" if gocode present in current path
	let l:match = matchstr(l:current_path,"gocode")
	let g:proj_name = ""
	if (l:match=="gocode")
		let g:proj_name = GOPrintProject()
	endif
	if (!empty(g:proj_name))
		let l:cscope_dir = "~/.vim/cscope_dir/"
		let l:db_path = l:cscope_dir.g:proj_name.".out"
		if (empty(glob(l:db_path)))
			call CscopeCreate()
		endif
		if (!empty(glob(l:db_path)))
			exe ":silent cs add ".l:db_path
		endif
	endif
endfunction

function! CscopeCreate()
	let l:prog_name = ""
	let l:current_path = getcwd() 
	" if gocode present in current path
	let l:match = matchstr(l:current_path,"gocode")
	if (l:match=="gocode")
		exe ":silent !create-cscope.sh -l go"
	endif
	" per ruby
	"if (l:match=="rubycode")
		"exe ":!create-cscope.sh -l go"
	"endif
endfunction


function! VisualSelection(direction) range
		let l:saved_reg = @"
		execute "normal! vgvy"

		let l:pattern = escape(@", '\\/.*$^~[]')
		let l:pattern = substitute(l:pattern, "\n$", "", "")

		if a:direction == 'b'
				execute "normal ?" . l:pattern . "^M"
		elseif a:direction == 'gv'
				call CmdLine("vimgrep " . '/'. l:pattern . '/' . ' **/*.')
		elseif a:direction == 'replace'
				call CmdLine("%s" . '/'. l:pattern . '/')
		elseif a:direction == 'f'
				execute "normal /" . l:pattern . "^M"
		endif

		let @/ = l:pattern
		let @" = l:saved_reg
endfunction


" Returns true if paste mode is enabled
function! HasPaste()
		if &paste
				return 'PASTE MODE	'
		en
		return ''
endfunction

" Don't close window, when deleting a buffer
command! Bclose call <SID>BufcloseCloseIt()
function! <SID>BufcloseCloseIt()
	 let l:currentBufNum = bufnr("%")
	 let l:alternateBufNum = bufnr("#")

	 if buflisted(l:alternateBufNum)
		 buffer #
	 else
		 bnext
	 endif

	 if bufnr("%") == l:currentBufNum
		 new
	 endif

	 if buflisted(l:currentBufNum)
		 execute("bdelete! ".l:currentBufNum)
	 endif
endfunction
"----------------------------------------------------------

"------- MANAGE COPY PASTE CLIPBOARD
"Copy & paste to system clipboard with <Space>p and <Space>y
"vmap <Leader>y "+y
"vmap <Leader>d "+d
"nmap <Leader>p "+p
"nmap <Leader>P "+P
"vmap <Leader>p "+p
"vmap <Leader>P "+P

"----- AG
nnoremap ,a :Ag!<Space>
"search test
"nnoremap <Leader>a vey :Ag! """<CR> 
nnoremap <silent><Leader>ag * :AgFromSearch<CR> 

"----- GO SETTINGS
let g:go_fmt_command = "gofmt"

"----- RUBYSETTINGS
set omnifunc=rubycomplete#Complete 
let g:rubycomplete_buffer_loading = 1 
let g:rubycomplete_classes_in_global = 1 
if has("autocmd")
	autocmd FileType ruby set omnifunc=rubycomplete#Complete
	autocmd FileType ruby let g:rubycomplete_buffer_loading=1
	autocmd FileType ruby let g:rubycomplete_classes_in_global=1
endif 

"---------------FUNCTIONS
"function! 
function! CommandLinePluginInstall()
	PluginInstall
	qall
endfunc

function! VerticalShrink()
	vertical resize -15
endfunc

function! VerticalEnlarge()
	vertical resize +15
endfunc

"function! ListOtherBuffers()
	"let g:current_buffer = bufnr('%')
	"echo g:current_buffer
	"let idx = 1
	"while idx <= bufnr("$")
		"if buflisted(idx)
			"if idx != g:current_buffer
				"let num_bufs += 1
		"endif
		"let idx += 1
	"endwhile
"endfunc

function! WinMoveRight()
	
endfunc
function! WinMoveLeft()
endfunc

function! CountWindows()
	let nwin = winnr('$')
	echo nwin
endfunc
function! CloseBuffer()
	call CloseNerdTreeTagList()
	if(CountListedBuffers()==1)
		exit
	else 
		bdelete
		syntax enable	"to resolv syntax off behavior by MBE
	endif
endfunction

function! SearchAg()
	normal vey,a
	"normal! *
endfunction

function! CountListedBuffers()
		let num_bufs = 0
		let idx = 1
		while idx <= bufnr("$")
				if buflisted(idx)
						let num_bufs += 1
				endif
				let idx += 1
		endwhile
		return num_bufs
endfunction

function! SwitchNerdTreeTagList()
		"if NerdTree open then close it and open Tlist
		if(exists("b:NERDTreeRoot") ==1)
			NERDTreeClose
			TlistOpen
			else 
			" If taglist window is open then close it and open NerdTree
			 let winnum = bufwinnr(g:TagList_title)
				if winnum != -1
					TlistClose
					NERDTreeFind
				else
					"if Nothing open open Tlist
					TlistOpen
				endif
		endif
endfunction

function! CloseNerdTreeTagList()
		if(exists("b:NERDTreeRoot") ==1)
			NERDTreeClose
		else
		" If taglist window is open then close it and open NerdTree
			 let winnum = bufwinnr(g:TagList_title)
				if winnum != -1
					TlistClose
				endif
		endif
endfunction

function! NumberToggle()
	if(&relativenumber == 1)
		set norelativenumber
	else
		set relativenumber
	endif
endfunction

function! Stab()
	let l:tabstop = 1 * input('set tabstop = softtabstop = shiftwidth = ')
	if l:tabstop > 0
		let &l:sts = l:tabstop
		let &l:ts = l:tabstop
		let &l:sw = l:tabstop
	endif
	call SummarizeTabs()
endfunction

function! SummarizeTabs()
	try
		echohl ModeMsg
		echon 'tabstop='.&l:ts
		echon ' shiftwidth='.&l:sw
		echon ' softtabstop='.&l:sts
		if &l:et
			echon ' expandtab'
		else
			echon ' noexpandtab'
		endif
	finally
		echohl None
	endtry
endfunction

function! EnableSyntax()
	if exists("g:syntax_on")
		echo 'syntax disabled'
		syntax off
	else
		echo 'syntax enabled'
		syntax enable
	endif
endfunction

"
" bootcamp
"

function! GOPrintProject()
		let l:current_path = getcwd()
		let l:godir= $GOPATH
		let l:git_dir = "src/github.com"
		let l:proj_rel_path = StringDifference(l:current_path,l:godir)
		let l:proj_name = StringDifference(l:proj_rel_path,l:git_dir)
		return l:proj_name
endfunction

function! StringDifference(str1,str2)
	let l:str1_tmp = split(a:str1,'\zs')
	"echo l:str1_tmp
	let l:str2_tmp = split(a:str2,'\zs')
	"echo l:str2_tmp
	let l:diff = []
	let c = 0
	let l:lenght = strlen(a:str2)
	"echom l:lenght
	let same = 1
	"while c < lenght && same = true
	while same == 1
		if c < l:lenght
			if l:str1_tmp[c] != l:str2_tmp[c]
				let same = 0
			endif
		else
			let same = 0
		endif
		let c += 1
	endwhile
	while c < strlen(a:str1)
		call add(l:diff,l:str1_tmp[c])
		let c +=1
	endwhile
	let l:diff_join = join(l:diff,"")
	return l:diff_join
endfunction

function! Assign(foo)
 let foo_tmp = a:foo
	 let foo_tmp = "Yep"
	echom foo_tmp
endfunction
