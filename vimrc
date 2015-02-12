"============ Summary =================

"D_Default						CM_Commands

"PL_Vundle						LN_Java

"FM_Formatting				LN_Ruby

"CS_Colorscheme				LN_Golang

"OV_OpeningVIM				LN_Python

"FC_Fileconf					PL_Ultisnips

"FF_FFunctions				PL_YouCompleteMe

"WS_Windows						PL_Airline

"BF_Buffer						PL_Ag

"TB_Tabs							PL_Eclim

"HI_Highlights				FN_Functions

"IM_InsertMode				TS_TestSection

"SR_SearchReplace

"======================================



"
"PL_Vundle
"
source	~/.vim/vundle-plugins.vim

"======================================


"
"D_Default
"
set shortmess=a
set cmdheight=2
syntax on
"enable number in page
set number
set relativenumber
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
" Remember info about open buffers on close
set viminfo^=%

set cinoptions+=(0

"======================================


"
"FM_Formatting
"
"ruler setting
set rulerformat=%-14.(%l,%c%V%)\ %P
"use the same symbol as TextMate for tabstop and EOL
set listchars=tab:▸\ ,eol:¬

"set mapleader space and not \ 
let mapleader = "\<Space>"

"======================================


"
"FC_Fileconf
"
"fast save quit
nnoremap <Leader>w :w<CR>
nnoremap <Leader>q :q<CR>
"Enter visual line mode with <Space><Space>
nmap <Leader><Leader> V
"shortcut to rapidly toggle 'set list'
nmap <leader>l :set list!<CR>
" Quickly edit/reload the vimrc file
nmap <silent> <leader>vv :vs <CR> -
"nmap <silent> <leader>e :e 
nmap <silent> <leader>ev :e ~/.vimrc<CR>
nmap <silent> <leader>vu :vs ~/.vim/vundle-plugins.vim<CR>
nmap <silent> <leader>sv :so $MYVIMRC<CR>

"======================================


"
"CS_Colorscheme
"
colorscheme molokai
hi MatchParen			 ctermfg=208	ctermbg=233 cterm=bold

"======================================


"
"IM_InsertMode
"
inoremap <C-l> <right>
inoremap <C-h> <left>

"======================================


"
"CM_Commands
"
"set tabstop, softtabstop and shiftwidth to the same value
command! -nargs=* Stab call Stab()
command! Home execute "e ~/"
command! CpMan :call CscopeProjectManagement()
command! CpCrate :call CscopeCreate()
command! CpUpdate	:call CscopeCreate()

"======================================


"
"WS_Windows
"
nnoremap <C-j> <C-w>j 
nnoremap <C-k> <C-w>k
nnoremap <C-h> <C-w>h<CR>
nnoremap <C-l> <C-w>l<CR>
nnoremap <silent> <Leader>rl :call VerticalShrink()<CR>
nnoremap <silent> <Leader>rh :call VerticalEnlarge()<CR>

"======================================


"
"FF_FFunctions
"Show variables and methods
map <F9> :TagbarToggle<CR>
map <F4> :NERDTreeToggle<CR>
"change number settings
map <F12> :call NumberToggle()<CR>

"======================================


"
"BF_Buffer
"
let mapleader = "\,"
" cycle back/forward recently viewed buffers
nnoremap <leader>l :bnext<cr>
nnoremap <leader>h :bprevious<cr>
nnoremap <leader>d :Bclose<cr>
map <Leader>bo :BufExplorer<CR>

"======================================


"
"TB_Tabs
"
let mapleader = "\<Tab>"
nnoremap <leader>n :tabnew<cr>
nnoremap <leader>l :tabnext<cr>
nnoremap <leader>h :tabprevious<cr>
"map <leader>o :tabonly<cr>
nnoremap <leader>d :tabclose<cr>
" Opens a new tab with the current buffer's path
map <leader>e :tabedit <c-r>=expand("%:p:h")<cr>/

"======================================


"
"HI_Highlights
"
"disable highlight when enter insert mode
autocmd InsertEnter * :setlocal nohlsearch
autocmd InsertLeave * :setlocal hlsearch

"======================================


"
"SR_SearchReplace
let mapleader = "\<Space>"
"replace current word in the next n lines
nnoremap <leader>tr :call CopyReplace()<cr>
nnoremap <leader>r :call Replace()<cr>

"======================================



"
"OV_OpeningVIM
"
" Return to last edit position when opening files (You want this!)
autocmd BufReadPost *
		 \ if line("'\"") > 0 && line("'\"") <= line("$") |
		 \	 exe "normal! g`\"" |
		 \ endif

"======================================


"
"LN_Java
"
let mapleader = "\<Space>"
au FileType java nmap <Leader>jc :JavaCorrect<cr>

"======================================


"
"LN_Ruby
"
au BufRead,BufNewFile *.rb set filetype=ruby
set omnifunc=rubycomplete#Complete 
let g:rubycomplete_buffer_loading = 1 
let g:rubycomplete_classes_in_global = 1 
if has("autocmd")
	autocmd FileType ruby set omnifunc=rubycomplete#Complete
	autocmd FileType ruby let g:rubycomplete_buffer_loading=1
	autocmd FileType ruby let g:rubycomplete_classes_in_global=1
endif 

"======================================

"
"LN_Clang
"
au FileType c nmap <Leader>gr :!make && ./main<cr>

"======================================

"
"LN_Golang
"
"let g:go_fmt_command = "gofmt"

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
"
let g:go_highlight_functions = 1
let g:go_highlight_methods = 1
let g:go_highlight_structs = 1
let g:go_highlight_operators = 1
let g:go_highlight_extra_types = 1

"======================================


"
"LN_Python
"
au FileType python set tabstop=4|set shiftwidth=4|set softtabstop=4|set noexpandtab

"======================================


"
"PL_Airline
"
" Enable the list of buffers
let g:airline#extensions#tabline#enabled = 1
" Show just the filename
let g:airline#extensions#tabline#fnamemod = ':t'

"======================================


"
"PL_Ultisnips
"
let g:UltiSnipsSnippetDirectories = ["UltiSnips"]
let g:UltiSnipsExpandTrigger = "<C-j>"
let g:UltiSnipsListSnippets = "<C-tab>"
let g:UltiSnipsJumpForwardTrigger = "<C-j>"
let g:UltiSnipsJumpBackwardTrigger = "<C-k>"

"======================================


"
"PL_YouCompleteMe
"
let g:ycm_autoclose_preview_window_after_completion = 1

"======================================


"
"PL_Ag
"
nnoremap ,a :Ag!<Space>
"search test
"nnoremap <Leader>a vey :Ag! """<CR> 
nnoremap <silent><Leader>ag * :AgFromSearch<CR> 

"======================================


"
"PL_Eclim
"
let g:EclimCompletionMethod = 'omnifunc'

"======================================


"
"FN_Functions
"

function! CopyReplace()
" corretto.. cambia solo colore della parola evideziata 	

	"salvo la parola ricercata
	let l:old_word = expand("<cword>")

	highlight MyGroup ctermbg=46 ctermfg=0"
	"exe ":highlight MyGroup ctermbg=46 ctermfg=0"
	"exe ":let m = matchadd(\"MyGroup\",\"" . l:old_word . "\")"
	"exe ":call matchdelete(l:match)"
	

	"numero di righe da copiare e modificare
	let l:rel_line= 1 * input('rel_lines:')

	let l:yank_lines = l:rel_line + 1
	
	"nuova parola
	let l:new_word= input('R word:')

	"copio le righe
	exe ":yank r " . l:yank_lines 


	let l:curr_line = line('.')
	echo "current line: " . l:curr_line
	"let l:output_line = l:curr_line + l:yank_lines - 1
	let l:output_line = l:curr_line + l:rel_line

	"spostati di n linee
	call cursor(l:output_line ,0)

	"stampa registro
	exe "normal! \"rp"


	"comando dim modifica
	exe ":.,.+" . l:yank_lines."s/" . l:old_word . "/" . l:new_word . "/cg"

endfunction

function! Replace()
" corretto.. cambia solo colore della parola evideziata 	

	"salvo la parola ricercata
	let l:old_word = expand("<cword>")

	highlight MyGroup ctermbg=46 ctermfg=0"
	"exe ":highlight MyGroup ctermbg=46 ctermfg=0"
	"exe ":let m = matchadd(\"MyGroup\",\"" . l:old_word . "\")"
	"exe ":call matchdelete(l:match)"
	

	"numero di righe da copiare e modificare
	let l:rel_line= 1 * input('rel_lines:')

	let l:yank_lines = l:rel_line + 1
	
	"nuova parola
	let l:new_word= input('R word:')

	"comando dim modifica
	exe ":.,.+" . l:yank_lines."s/" . l:old_word . "/" . l:new_word . "/cg"

endfunction

function! ChangeWord(yank_lines,old_word)
endfunction

function! CmdLine(str)
		exe "menu Foo.Bar :" . a:str
		emenu Foo.Bar
		unmenu Foo
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

"TS_TestSection
"------------------- Test-section  ----------------------- 
"

function! CatchUri()

endfunction

"---------------------------------------------------- 
