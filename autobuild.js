const fs = require('fs')
const child_process = require('child_process');
const Stream = require('stream');
const { exit } = require('process');

const url = 'https://github.com/CrowCpp/Crow/releases/download/v1.2.1.2/Crow-1.2.1-Linux.deb'

async function main(){

    const uid = process.geteuid()
    if(uid != 0){
        console.log('please run sudo this script!');
        return
    }

    console.log('start downloading dependencies');

    try {
        child_process.execSync('sudo apt update -y', {stdio: 'inherit'})
        child_process.execSync('sudo apt install libasio-dev libpqxx-dev cmake make gcc -y', {stdio: 'inherit'})
        
    } catch (error) {
        console.error(error);
        return
    }

    try {
        child_process.execSync('dpkg -l | grep crow', {stdio: 'inherit'})
        console.log('crow installed');
        
    } catch (error) {
        console.log('crow not installed, start to download');
        const response = await fetch(url)
        if(!response.ok){
            console.log('download failed, exiting');
            return
        } 

        const fileStream = fs.createWriteStream('./Crow-1.2.1-Linux.deb')
        Stream.pipeline(response.body, fileStream, (err) => {
            if(err){
                console.error(err);
                fileStream.emit('error')
                return 
            }else{
                console.log('pipeline successful');
                
            }
        })
        fileStream.on('finish', () => {
            console.log('downloading successful');
            
            try {
                child_process.execSync('sudo apt install "./Crow-1.2.1-Linux.deb" -y', {stdio: 'inherit'})
            } catch (error) {
                console.log('installing failed');
                exit(1)
            }
        })

        fileStream.on('error', () => {
            console.log('downloading failed');
            exit(1)
        })

    }

    


    if(fs.existsSync('./build')){
        fs.rmdirSync('./build')
    }

    fs.mkdirSync('./build') 



}

main()