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
                            cmakeBuild buildDir: 'build', installation: 'InSearchPath', buildType: 'MinSizeRel', cmakeArgs: '-G Ninja'
                            cmake workingDir: 'build', arguments: '--build . --target package', installation: 'InSearchPath'
                        }
                    }
                }
                post { 
                    always {
                        archiveArtifacts artifacts: 'build/umdoc-*.zip,build/umdoc-*.deb'
                    }
                } 
            }
        }
    }
}

