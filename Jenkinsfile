pipeline {
    agent none
    stages {
        stage('All') {
            matrix {
                agent {
                    label "${platform}"
                }
                axes {
                    axis {
                        name 'platform'
                        values 'ubuntu20.04-x86_64', 'ubuntu18.04-x86_64', 'windows10-x64', 'windows10-x86'
                    }
                }
                stages {
                    stage('Build') {
                        steps {
                            script {
                                if (isUnix()) {
                                    sh 'rm -rf build/umdoc-*.zip build/umdoc-*.deb build/Doc/umdoc-*.pdf'
                                }
                                else {
                                    bat 'del build\\umdoc-*.zip'
                                }

                                cmakeBuild buildDir: 'build', installation: 'InSearchPath', buildType: 'MinSizeRel', cmakeArgs: '-G Ninja'
                                cmake workingDir: 'build', arguments: '--build . --target package', installation: 'InSearchPath'
                            
                                if (platform == 'ubuntu20.04-x86_64') {
                                    dir('build/Doc') {
                                        archiveArtifacts artifacts: 'umdoc-*.pdf'
                                    }
                                }
                                
                                dir('build') {
                                    archiveArtifacts artifacts: 'umdoc-*.zip,umdoc-*.deb'
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

