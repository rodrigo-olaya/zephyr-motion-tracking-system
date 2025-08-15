import { useEffect, useState } from 'react'
import axios from 'axios'

const apiUrl = import.meta.env.VITE_API_URL as string;

const GetImuData = () => {

    const [values, setValues] = useState<string[]>([])

    const fetchAllData = async () => {
        try {
        const response = await axios.get(apiUrl)
        if (Array.isArray(response.data)) {
            setValues(response.data)
        } else {
            console.error('Unexpected data format', response.data)
        }
    } catch (error) {
        console.error('Failed to fetch', error)
    }
}
    
    useEffect(() => {
        fetchAllData()
        const interval = setInterval(fetchAllData, 1000)
        return () => clearInterval(interval)
    }, [])
    return (
        <div>
            <p>{values.join(', ')}</p>
        </div>
    )
}
export default GetImuData;