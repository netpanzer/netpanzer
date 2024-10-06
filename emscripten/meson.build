em_preamble = files('preamble.js')
em_shell = files('web.html')

if host_machine.system() == 'emscripten'
    install_data(
        files(
#            'background.webp',
#            'scythe.webp',
#            'favicon.ico',
        ),
        install_dir : bindir,
        install_tag : 'runtime',
    )
endif
