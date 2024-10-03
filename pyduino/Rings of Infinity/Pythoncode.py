import serial
import time
import pandas as pd # import pandas lib as pd
import numpy as np
import os,glob #For 'clear' page

def name_all_cripto():
    a=[]
    a=a+['']
    a=a+['BNB']
    a=a+['BTC']
    a=a+['ETH']
    a=a+['USDT']
    a=a+['USDC']
    a=a+['DOGE']
    return a


def read_users(Source):
    # تعریف یک دیکشنری از یوزرنیم‌ها و پسوردها
    accounts = {}
    if Source=='User':
        # خواندن اطلاعات از فایل
        file = np.loadtxt("./Data/Data_Open.txt", dtype='str', delimiter='\t') 
        for row in file:
            username,ID ,BNB, BTC,ETH,USDT,USDC,DOGE = row
            accounts[username.strip()] =ID,BNB, BTC,ETH,USDT,USDC,DOGE.strip()
    elif Source=='ID':
        # خواندن اطلاعات از فایل
        file = np.loadtxt("./Data/Data_Open.txt", dtype='str', delimiter='\t') 
        for row in file:
            username,ID ,BNB, BTC,ETH,USDT,USDC,DOGE = row
            accounts[ID.strip()] =username,BNB, BTC,ETH,USDT,USDC,DOGE.strip()
    elif Source=='Pass':  
        file = np.loadtxt("./Data/Data_Close.txt", dtype='str', delimiter='\t') 
        for row in file:
            username,Password = row
            accounts[username.strip()] =Password.strip()
    return accounts




def get_crypto_price(crypto_symbol, vs_currency='USD'):
    import requests
    url = 'https://pro-api.coinmarketcap.com/v1/cryptocurrency/quotes/latest'
    parameters = {
        'symbol': crypto_symbol,
        'convert': vs_currency
    }
    headers = {
        'Accepts': 'application/json',
        'X-CMC_PRO_API_KEY': 'ac00d905-5650-41a0-8d14-3f3668b775d0'
    }

    response = requests.get(url, headers=headers, params=parameters)
    data = response.json()

    try:
        price = data['data'][crypto_symbol]['quote'][vs_currency]['price']
        return price
    except KeyError:
        raise ValueError(f"Invalid response or cryptocurrency symbol '{crypto_symbol}' not found")



def get_usd_to_irr_rate():
    import requests
    api_key = 'freeQZcYrXie3qqLXM45G4kS3cUCRRwW'
    url = f'http://api.navasan.tech/latest/?api_key={api_key}&item=usd_sell'
    response = requests.get(url)
    if response.status_code == 200:
        data = response.json()
        if 'usd_sell' in data:
            return data['usd_sell']['value']
        else:
            raise ValueError("The response does not contain the expected 'usd_sell' field.")
    else:
        response.raise_for_status()

def view_wallet(ser,Username):
    usd_to_irr_rate = get_usd_to_irr_rate()
    Message_Show_L1(ser,f"1 USD:{usd_to_irr_rate} IRR")
    toman=int(int(float(usd_to_irr_rate))/10)
    Message_Show_L2(ser,f"{toman} Toman")
    time.sleep(5.1)                                         # Optional: add a small delay to avoid high CPU usage
    Data_User=read_users('User')    
    Data_User=Data_User[Username]
    Token_name=name_all_cripto()
    Message_Show_L1(ser,f"you'r wallet is:")
    sum=0
    for i in range (len(Token_name)-1):
        pr=get_crypto_price(Token_name[i+1])
        sum+=float(pr)*float(Data_User[i+1])
        Message_Show_L1(ser,f"{Token_name[i+1]}: {round(float(Data_User[i+1]),3)}")
        Message_Show_L2(ser,f"{str(round(float(pr)*float(Data_User[i+1]),3))}$ {str(int(toman*float(pr)*float(Data_User[i+1])))}Toman")

        #print(f'{Token_name[i+1]}: \t{round(float(Data_User[i+1]),3)}\t{str(round(float(pr),3))} $\t{str(round(float(pr)*float(Data_User[i+1]),3))} $\t{str(int(toman*float(pr)*float(Data_User[i+1])))} Toman')
    toman_total=int(int(sum*float(usd_to_irr_rate))/10)
    #print(f'\nTotal:\t{sum} $\t{toman_total} Toman')
    Message_Show_L1(ser,f"Total: {sum:.2f}$")
    Message_Show_L2(ser,f"{toman_total}Toman")











def Request1(ser):
    Message_Show_L1(ser,"1.view wallet")
    Message_Show_L2(ser,"2.the transfer")
    message=0
    ser.write("request\n".encode('utf-8'))  # Send the command to Arduino to call RECOGNITION()
    while True:
        # Read any incoming message from Arduino
        if ser.in_waiting > 0:
            message = ser.readline().decode('utf-8').strip()
            print(f"LCD Message: {message}")  # Print all messages sent from Arduino
            if message=="1" or message=="2":
                break
    return int(message)



def Message_Show_L1(ser,Message_desired):
    #os.system('cls' if os.name == 'nt' else 'clear')
    incoming = "Empty"
    while incoming!=Message_desired:                                                # Ring of Infinity
        ser.write("Message_L1\n".encode('utf-8'))  # Send the command to Arduino to call RECOGNITION()
        ser.write(f"{Message_desired}\n".encode('utf-8'))    # Send the command to Arduino to read tag_id for tags(tag_id)
        incoming = ser.readline().decode('utf-8').strip()    # Read any incoming message from Arduino
        print(incoming)
        time.sleep(0.1)                                         # Optional: add a small delay to avoid high CPU usage

def Message_Show_L2(ser,Message_desired):
    #os.system('cls' if os.name == 'nt' else 'clear')
    incoming = "Empty"
    while incoming!=Message_desired:                                                # Ring of Infinity
        ser.write("Message_L2\n".encode('utf-8'))  # Send the command to Arduino to call RECOGNITION()
        ser.write(f"{Message_desired}\n".encode('utf-8'))    # Send the command to Arduino to read tag_id for tags(tag_id)
        incoming = ser.readline().decode('utf-8').strip()    # Read any incoming message from Arduino
        print(incoming)
        time.sleep(0.1)                                         # Optional: add a small delay to avoid high CPU usage






################################################################################################################################
#####                                                Recive_pool_MetaMask                                                   ####
################################################################################################################################

def sign_server(Username,dataframe1):
    Data_User=read_users('User')
    while True:                                                 # Ring of Infinity
        Password = input("Enter your Sign: ")
        if read_users('Pass')[Username] == Password:
            Data_User=Data_User[Username]
            Data_ID=read_users('ID')[Data_User[0]]
            Token_name=name_all_cripto()
            print(Token_name)
            print(Data_User)
            print(Data_ID)
            print("Login successful!")
            break
        else:
            print("The Sign is incorrect")

################################################################################################################################
#####                                                Recive_pool_MetaMask                                                   ####
################################################################################################################################
def Recive_pool_MetaMask():
    from web3 import Web3
    bsc = "https://bsc-dataseed.binance.org/"
    while True:                                                 # Ring of Infinity
        web3 = Web3(Web3.HTTPProvider(bsc))
        if web3.is_connected():
            print("Connected to BSC")
            break
        else:
            print("Connection failed")
    private_key = "173a3a5b73f295477a9b1b912e8f012fe2438611c7c35ae11d4ab4ee4f550f41"#مهم ترین چیز
    account = web3.eth.account.from_key(private_key)
    address = account.address
    print(f"Connected to address MetaMask: {address}")
    def Amount_token(contract_address):
        if(contract_address=="BNB"):
            balance = web3.eth.get_balance(address)
            balance_in_bnb = web3.from_wei(balance, 'ether')
            Amount=balance_in_bnb
        else:
            abi = [
                {
                    "constant": True,
                    "inputs": [{"name": "_owner", "type": "address"}],
                    "name": "balanceOf",
                    "outputs": [{"name": "balance", "type": "uint256"}],
                    "payable": False,
                    "stateMutability": "view",
                    "type": "function"
                }
            ] 
            contract = web3.eth.contract(address=contract_address, abi=abi)
            balance = contract.functions.balanceOf(address).call()
            Amount=balance / (10 ** 18)
        return str(Amount)
    
    Amounts = []                                                # Defining list of strings
    Amounts.append(Amount_token("BNB"))                                       #BNB
    Amounts.append(Amount_token("0x7130d2A12B9BCbFAe4f2634d864A1Ee1Ce3Ead9c"))#BTC
    Amounts.append(Amount_token("0x2170Ed0880ac9A755fd29B2688956BD959F933F8"))#ETH
    Amounts.append(Amount_token("0x55d398326f99059fF775485246999027B3197955"))#USDT
    Amounts.append(Amount_token("0x8AC76a51cc950d9822D68b83fE1Ad97B32Cd580d"))#USDC
    Amounts.append(Amount_token("0xbA2aE424d960c26247Dd6c32edC70B295c744C43"))#DOGE
    with open("Amount.txt", 'w') as f:                          # create a total Amount pool
        f.writelines(s + '\n' for s in Amounts)                 # write Amounts of pool
    f.close()
################################################################################################################################
#####                                                           Tag                                                         ####
################################################################################################################################
def Tag(tag_id,Data_user,ser):
    ser.write("RECOGNITION\n".encode('utf-8'))         # Send the command to Arduino to call tags()
    while True:                                                 # Ring of Infinity
        if ser.in_waiting > 0:                                  # if see incoming message from Arduino
            message = ser.readline().decode('utf-8').strip()    # Read any incoming message from Arduino
            len_message=len(message)                            # calculate lenth of message of arduino for end element * and #
            print(f"LCD Message: {message}")                    # Print all messages sent from Arduino
            ###################################################
            #####              Recognition Tag             ####
            ###################################################
            for index, row in Data_user.iterrows():             # check all tag of avilable
                if message == "tagId is : " + str(row['Tag']):  # if see tag message from arduino
                    tag_id=int(row['id'])                       # which id of tag detected
                    ser.write("tags\n".encode('utf-8'))         # Send the command to Arduino to call tags()
                    ser.write(f"{tag_id}\n".encode('utf-8'))    # Send the command to Arduino to read tag_id for tags(tag_id)
            ###################################################
            #####          Check Password Correct          ####
            ###################################################
            if tag_id != 0:                                     # after detect tag
                if message == str(Data_user.password[tag_id])+"#":# if message from arduino equal to pass user
                    ser.write("passwords_correct\n".encode('utf-8'))# Send the command to Arduino to call passwords_correct()
                    while message!="Just Moments...":
                        message = ser.readline().decode('utf-8').strip()    # Read any incoming message from Arduino
                        print(f"LCD Message: {message}")                    # Print all messages sent from Arduino   
                    break                                      # Get out to Ring of Infinity
            ###################################################
            #####               Password Wrong             ####
            ###################################################
            if len_message != 0:                                # if sent message 
                if message[-1] == "#":                          # if end of element of message of arduino was #
                    ser.write("passwords_wrong\n".encode('utf-8'))# Send the command to Arduino to call passwords_wrong()
            ###################################################
            #####               Password Clear             ####
            ###################################################
            if len_message != 0:                                # if sent message 
                if message[-1] == "*":                          # if end of element of message of arduino was *
                    ser.write("password_clear\n".encode('utf-8'))# Send the command to Arduino to call password_clear()
        time.sleep(0.1)                                         # Optional: add a small delay to avoid high CPU usage
    return tag_id                                               # return id of tag detected after correct password


################################################################################################################################
#####                                                          MAIN                                                         ####
################################################################################################################################
def main():
    ser = serial.Serial('COM7', 115200)                                                 # Adjust port and baud rate
    while True:                                                                         # Ring of Infinity
        Data_user = pd.read_excel('Data_user.xlsx', usecols=['id', 'Tag', 'password']) # read by default 1st sheet of an excel file
        tag_id=0                                                                        # define id of detect with tag
        tag_id=Tag(tag_id,Data_user,ser)                                           # detect tag and user pass and use * and #
        Recive_pool_MetaMask()
        sign_server(str(Data_user.Tag[tag_id]),ser)
        request=Request1(ser)
        if request==1:
            view_wallet(ser,str(Data_user.Tag[tag_id]))


        time.sleep(5.1)                                         # Optional: add a small delay to avoid high CPU usage
        print("i am ready")
        Message_Show_L1(ser,"I AM Inevitable")
        time.sleep(5.1)                                         # Optional: add a small delay to avoid high CPU usage





if __name__=="__main__":
    main()
