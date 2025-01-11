em_preamble = files('preamble.js')
em_shell = files('web.html')

if host_machine.system() == 'emscripten'
    install_data(
        files(
            # TODO static files will go here
        ),
        install_dir : bindir,
        install_tag : 'runtime',
    )
endif
